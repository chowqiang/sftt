/*
 * Copyright (C)  2020-2021 Min Zhou <zhoumin@bupt.cn>, all rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <assert.h>
#include <libgen.h>
#include <stdio.h>
#include <stdbool.h>
#include "autoconf.h"
#include "debug.h"
#include "dlist.h"
#include "file.h"
#include "file_trans.h"
#include "mkdirp.h"
#include "mem_pool.h"
#include "net_trans.h"
#include "progress_viewer.h"
#include "response.h"
#include "trans.h"
#include "utils.h"

int send_file_content_by_get_resp(int fd, struct sftt_packet *resp_packet,
	struct get_resp *resp, int flags)
{
	DBUG_ENTER(__func__);

	DBUG_RETURN(send_get_resp(fd, resp_packet, resp, RESP_OK, flags));
}

int send_file_name_by_get_resp(int fd, char *path, char *fname,
	struct sftt_packet *req_packet, struct sftt_packet *resp_packet,
	struct get_resp *resp, int flags)
{
	DBUG_ENTER(__func__);

	struct common_resp *com_resp;
	int ret;

	if (is_dir(path))
		resp->data.entry.type = FILE_TYPE_DIR;
	else
		resp->data.entry.type = FILE_TYPE_FILE;

	resp->data.entry.mode = file_mode(path);
	strncpy((char *)resp->data.entry.content, fname, FILE_NAME_MAX_LEN);
	resp->data.entry.this_size = strlen(fname);

	resp->need_reply = 1;
	send_get_resp(fd, resp_packet, resp, RESP_OK, flags);

	ret = recv_sftt_packet(fd, req_packet);
	if (ret == -1) {
		DEBUG((DEBUG_ERROR, "recv sftt packet failed!\n"));
		DBUG_RETURN(-1);
	}

	com_resp = req_packet->obj;
	if (com_resp->status != RESP_OK) {
		DEBUG((DEBUG_ERROR, "peer cannot handle the file name|path=%s\n", path));
		ret = -1;
	}
	mp_free(g_mp, com_resp);

	DBUG_RETURN(ret);
}

int send_file_md5_by_get_resp(int fd, char *path, struct sftt_packet *resp_packet,
	struct get_resp *resp)
{
	DBUG_ENTER(__func__);

	int ret;

	assert(!is_dir(path));

	resp->data.entry.total_size = file_size(path);
	DEBUG((DEBUG_INFO, "file size|total.size=%d\n", resp->data.entry.total_size));

	ret = md5_file(path, resp->data.entry.content);
	if (ret == -1) {
		DEBUG((DEBUG_ERROR, "get file md5 failed|path=%s\n", path));
		DBUG_RETURN(-1);
	}
	DEBUG((DEBUG_INFO, "file=%s|md5=%s\n", path, resp->data.entry.content));

	resp->data.entry.this_size = strlen((char *)resp->data.entry.content);
	DEBUG((DEBUG_INFO, "md5 len|len=%d\n", resp->data.entry.this_size));

	resp->need_reply = 1;

	DBUG_RETURN(send_get_resp(fd, resp_packet, resp, RESP_OK, REQ_RESP_FLAG_NONE));
}

int send_file_by_get_resp(int fd, char *path, char *fname,
	struct sftt_packet *req_packet,
	struct sftt_packet *resp_packet, struct get_resp *resp)
{
	DBUG_ENTER(__func__);

	struct common_resp *com_resp;
	int ret, is_last, flags;
	long read_size;
	FILE *fp;
	bool need_stop;

	is_last = resp->data.file_idx == resp->data.total_files - 1;
	need_stop = is_last && is_dir(path);
	if (need_stop)
		flags = REQ_RESP_FLAG_STOP;
	else
		flags = REQ_RESP_FLAG_NONE;

	if (is_dir(path)) {
		DEBUG((DEBUG_INFO, "send dir name|path=%s|fname=%s\n", path, fname));
		DBUG_RETURN(send_file_name_by_get_resp(fd, path, fname, req_packet, resp_packet, resp, flags));
	}

	DEBUG((DEBUG_INFO, "send file name|path=%s|fname=%s\n", path, fname));
	ret = send_file_name_by_get_resp(fd, path, fname, req_packet, resp_packet, resp, REQ_RESP_FLAG_NONE);
	if (ret == -1) {
		DEBUG((DEBUG_ERROR, "send file name failed|path=%s\n", path));
		DBUG_RETURN(-1);
	}

	DEBUG((DEBUG_INFO, "send file md5|path=%s|fname=%s\n", path, fname));
	ret = send_file_md5_by_get_resp(fd, path, resp_packet, resp);
	if (ret == -1) {
		DEBUG((DEBUG_ERROR, "send file md5 failed|path=%s\n", path));
		DBUG_RETURN(-1);
	}

	DEBUG((DEBUG_INFO, "recv md5 resp|path=%s|fname=%s\n", path, fname));
	ret = recv_sftt_packet(fd, req_packet);
	if (ret == -1) {
		printf("%s: recv sftt packet failed!\n", __func__);
		DBUG_RETURN(-1);
	}

	com_resp = req_packet->obj;
	if (com_resp->status == RESP_OK) {
		DEBUG((DEBUG_WARN, "file not changed and skip it|path=%s\n", path));
		mp_free(g_mp, com_resp);
		DBUG_RETURN(0);
	}
	mp_free(g_mp, com_resp);

	fp = fopen(path, "r");
	if (fp == NULL) {
		DEBUG((DEBUG_ERROR, "cannot open file|path=%s\n", path));
		DBUG_RETURN(-1);
	}

	resp->data.entry.total_size = file_size(path);
	read_size = 0;

	DEBUG((DEBUG_INFO, "begin to send file block\n"));

	do {
		ret = fread(resp->data.entry.content, 1, CONTENT_BLOCK_SIZE, fp);

		resp->data.entry.this_size = ret;
		read_size += ret;

		if (is_last && read_size == resp->data.entry.total_size)
			flags = REQ_RESP_FLAG_STOP;
		else
			flags = REQ_RESP_FLAG_NONE;

#ifdef CONFIG_RESP_PER_FILE_BLOCK
		resp->need_reply = 1;
#else
		resp->need_reply = 0;
#endif
		DEBUG((DEBUG_INFO, "send file block|len=%d|flags=%d\n", ret, flags));
		ret = send_file_content_by_get_resp(fd, resp_packet, resp, flags);
		if (ret == -1) {
			printf("%s: recv sftt packet failed!\n", __func__);
			break;
		}

		if (resp->need_reply) {
			DEBUG((DEBUG_INFO, "recv common resp\n"));
			ret = recv_sftt_packet(fd, req_packet);
			if (ret == -1) {
				DEBUG((DEBUG_ERROR, "recv sftt packet failed!\n"));
				break;
			}
			com_resp = req_packet->obj;
			if (com_resp->status != RESP_OK) {
				DEBUG((DEBUG_ERROR, "recv response failed!\n"));
				mp_free(g_mp, com_resp);
				break;
			}
			mp_free(g_mp, com_resp);
		}
	} while (read_size < resp->data.entry.total_size);

	if (!feof(fp))
		ret = -1;

	fclose(fp);

	DBUG_RETURN(ret);
}

int send_dir_by_get_resp(int fd, char *path, struct sftt_packet *req_packet,
	struct sftt_packet *resp_packet, struct get_resp *resp)
{
	DBUG_ENTER(__func__);

	struct dlist *file_list;
	struct dlist_node *node;
	int file_count;
	struct path_entry *entry;

	DEBUG((DEBUG_INFO, "send dir in ...|path: %s\n", path));

	file_list = get_path_entry_list(path, NULL);
	if (file_list == NULL) {
		DEBUG((DEBUG_ERROR, "cannot get file list please ensure the path"
				" is absolute!|path=%s\n", path));
		DBUG_RETURN(-1);
	}
	dlist_for_each(file_list, node) {
		entry = node->data;
		DEBUG((DEBUG_INFO, "entry->abs_path=%s\n", entry->abs_path));
	}

	file_count = dlist_size(file_list);
	DEBUG((DEBUG_INFO, "file list|file_count=%d\n", file_count));

	resp->data.total_files = file_count;
	resp->data.file_idx = 0;

	dlist_for_each(file_list, node) {
		entry = node->data;

		DEBUG((DEBUG_INFO, "send file|idx=%d|path=%s\n",
					resp->data.file_idx, entry->abs_path));

		if (send_file_by_get_resp(fd, entry->abs_path, entry->rel_path,
				req_packet, resp_packet, resp) == -1) {
			DEBUG((DEBUG_ERROR, "send file failed|path=%s\n",
					(char *)entry->abs_path));
		}

		DEBUG((DEBUG_INFO, "send file done|idx=%d|path=%s\n",
					resp->data.file_idx, entry->abs_path));

		resp->data.file_idx++;
	}

	DEBUG((DEBUG_INFO, "send dir out\n"));
	dlist_destroy(file_list);

	DBUG_RETURN(0);
}

int send_files_by_get_resp(int fd, char *path, struct sftt_packet *req_packet,
		struct sftt_packet *resp_packet, struct get_resp *resp)
{
	DBUG_ENTER(__func__);

	int ret;
	char *fname;

	if (is_file(path)) {
		DEBUG((DEBUG_INFO, "send single file|path=%s\n", path));

		resp->data.total_files = 1;
		resp->data.file_idx = 0;

		fname = get_basename(path);
		ret = send_file_by_get_resp(fd, path, fname, req_packet, resp_packet, resp);

		DEBUG((DEBUG_INFO, "send single file done|path=%s\n", path));

	} else {
		DEBUG((DEBUG_INFO, "send multi files|path=%s\n", path));

		ret = send_dir_by_get_resp(fd, path, req_packet, resp_packet, resp);

		DEBUG((DEBUG_INFO, "send multi files done|path=%s\n", path));
	}

	DBUG_RETURN(ret);
}

int send_trans_entry_by_put_req(int fd,
	struct sftt_packet *req_packet, struct put_req *req)
{
	DBUG_ENTER(__func__);

	req_packet->type = PACKET_TYPE_PUT_REQ;

	req_packet->obj = req;

	int ret = send_sftt_packet(fd, req_packet);
	if (ret == -1) {
		DEBUG((DEBUG_WARN, "send sftt packet failed!\n"));
		DBUG_RETURN(-1);
	}

	DBUG_RETURN(0);
}


int send_file_name_by_put_req(int fd, struct sftt_packet *req_packet,
		char *file, char *target, struct put_req *req)
{
	DBUG_ENTER(__func__);

	if (is_dir(file))
		req->data.entry.type = FILE_TYPE_DIR;
	else
		req->data.entry.type = FILE_TYPE_FILE;

	req->data.entry.mode = file_mode(file);
	strncpy((char *)req->data.entry.content, target, FILE_NAME_MAX_LEN);
	req->data.entry.this_size = strlen(target);

	req->need_reply = 1;

	DBUG_RETURN(send_trans_entry_by_put_req(fd, req_packet, req));
}

int send_file_md5_by_put_req(int fd, struct sftt_packet *req_packet, char *file,
	struct put_req *req)
{
	DBUG_ENTER(__func__);

	int ret;

	if (is_dir(file))
		DBUG_RETURN(0);

	ret = md5_file(file, req->data.entry.content);
	if (ret == -1)
		DBUG_RETURN(-1);

	req->data.entry.this_size = strlen((char *)req->data.entry.content);

	req->need_reply = 1;

	DBUG_RETURN(send_trans_entry_by_put_req(fd, req_packet, req));
}

int send_file_content_by_put_req(int fd,
	struct sftt_packet *req_packet, struct put_req *req)
{
	DBUG_ENTER(__func__);

	DBUG_RETURN(send_trans_entry_by_put_req(fd, req_packet, req));
}

int send_file_by_put_req(int fd, char *file, char *target, struct sftt_packet *req_packet,
		struct put_req *req, struct sftt_packet *resp_packet)
{
	DBUG_ENTER(__func__);

	struct put_resp *resp;
	int ret, is_last;
	FILE *fp;
	struct progress_viewer pv;
	char progress_info[128];
	long total_size = 0;
	long send_size = 0;
	long speed = 0;
	float progress;
	double start, now;
	int left_time;
	char speed_info[16];
	char left_time_info[16];
	char send_size_info[16];
	bool need_stop;
	int i = 0;

	// If it is the last file?
	is_last = req->data.file_idx == (req->data.total_files - 1);
	need_stop = is_last && is_dir(file);
	if (need_stop)
		req->flags = REQ_RESP_FLAG_STOP;
	else
		req->flags = REQ_RESP_FLAG_NONE;

	total_size = file_size(file);
	req->data.entry.total_size = total_size;

	// Send file name
	ret = send_file_name_by_put_req(fd, req_packet, file, target, req);
	if (ret == -1) {
		DEBUG((DEBUG_WARN, "send file name failed!\n"));
		DBUG_RETURN(-1);
	}

	// Recv response for creating file
	ret = recv_sftt_packet(fd, resp_packet);
	if (ret == -1) {
		DEBUG((DEBUG_WARN, "recv sftt packet failed!\n"));
		DBUG_RETURN(-1);
	}
	resp = resp_packet->obj;
	if (resp->status != RESP_OK) {
		DEBUG((DEBUG_WARN, "server create file failed!\n"));
		mp_free(g_mp, resp);
		DBUG_RETURN(-1);
	}
	mp_free(g_mp, resp);

	// Done when file is dir
	if (is_dir(file))
		DBUG_RETURN(0);

	start = get_double_time();
	start_progress_viewer(&pv, 1000 * 1000);

	// send md5 of file
	ret = send_file_md5_by_put_req(fd, req_packet, file, req);
	if (ret == -1) {
		DEBUG((DEBUG_WARN, "send md5 failed|file=%s\n", file));
		DBUG_RETURN(-1);
	}

	// recv response for checking md5 of file
	ret = recv_sftt_packet(fd, resp_packet);
	if (ret == -1) {
		DEBUG((DEBUG_WARN, "recv sftt packet failed|file=%s\n", file));
		DBUG_RETURN(-1);
	}
	resp = resp_packet->obj;
	if (resp->status == RESP_OK) {
		DEBUG((DEBUG_INFO, "file not changed: %s, skip ...\n", file));

		send_size = total_size;
		now = get_double_time();
		speed = send_size * 1.0 / (now - start);
		left_time = speed ? (total_size - send_size) / speed : 0;

		format_trans_speed(speed, speed_info, sizeof(speed_info));
		format_trans_size(send_size, send_size_info, sizeof(send_size_info));
		format_left_time(left_time, left_time_info, sizeof(left_time_info));
		progress = total_size ? (send_size * 1.0) / total_size : 1.0;
		snprintf(progress_info, 128, "%s    %d%% %s %s %s", basename(file),
				(int)(progress * 100), send_size_info, speed_info, left_time_info);

		stop_progress_viewer(&pv, progress_info);

		mp_free(g_mp, resp);
		DBUG_RETURN(0);
	}
	mp_free(g_mp, resp);

	DEBUG((DEBUG_DEBUG, "open file to read|file=%s\n", file));
	// open file for reading
	fp = fopen(file, "r");
	if (fp == NULL) {
		DEBUG((DEBUG_WARN, "open file failed|file=%s\n", file));
		DBUG_RETURN(-1);
	}

	req->data.entry.total_size = total_size;

	while (!feof(fp)) {
		ret = fread(req->data.entry.content, 1, CONTENT_BLOCK_SIZE, fp);
		req->data.entry.this_size = ret;
		need_stop = is_last && feof(fp);
		if (need_stop)
			req->flags = REQ_RESP_FLAG_STOP;
		else
			req->flags = REQ_RESP_FLAG_NONE;

#ifdef CONFIG_RESP_PER_FILE_BLOCK
		req->need_reply = 1;
#else
		req->need_reply = 0;
#endif
		if (need_stop)
			req->need_reply = 1;
		ret = send_file_content_by_put_req(fd, req_packet, req);
		if (ret == -1) {
			DEBUG((DEBUG_WARN, "send file content failed|file=%s\n", file));
			break;
		}
		DEBUG((DEBUG_INFO, "send file block|file=%s|idx=%d\n", file, i++));

		if (req->need_reply) {
			DEBUG((DEBUG_INFO, "recv common resp\n"));
			ret = recv_sftt_packet(fd, resp_packet);
			if (ret == -1) {
				DEBUG((DEBUG_ERROR, "recv sftt packet failed!\n"));
				break;
			}
			resp = resp_packet->obj;
			if (resp->status != RESP_OK) {
				DEBUG((DEBUG_ERROR, "recv response failed!\n"));
				mp_free(g_mp, resp);
				break;
			}
			mp_free(g_mp, resp);
		}

		send_size += req->data.entry.this_size;
		now = get_double_time();
		speed = send_size * 1.0 / (now - start);
		left_time = speed ? (total_size - send_size) / speed : 0;

		format_trans_speed(speed, speed_info, sizeof(speed_info));
		format_trans_size(send_size, send_size_info, sizeof(send_size_info));
		format_left_time(left_time, left_time_info, sizeof(left_time_info));
		progress = total_size ? (send_size * 1.0) / total_size : 1.0;
		snprintf(progress_info, 128, "%s    %d%% %s %s %s", basename(file),
				(int)(progress * 100), send_size_info, speed_info,
				left_time_info);
		show_progress(&pv, progress_info);

		if (send_size == total_size)
			stop_progress_viewer(&pv, progress_info);

	}

	if (!feof(fp)) {
		DEBUG((DEBUG_ERROR, "read file failed!|file=%s\n", file));
		ret = -1;
	}

	fclose(fp);

	DBUG_RETURN(ret);
}

int send_dir_by_put_req(int fd, char *path, char *target,
	struct sftt_packet *req_packet, struct put_req *req,
	struct sftt_packet *resp_packet)
{
	DBUG_ENTER(__func__);

	int ret, file_count;
	char tmp[FILE_NAME_MAX_LEN + 2];
	struct path_entry *entry;
	struct dlist *file_list;
	struct dlist_node *node;

	file_list = get_path_entry_list(path, NULL);
	if (file_list == NULL) {
		DEBUG((DEBUG_ERROR, "file list is null!\n"));
		DBUG_RETURN(-1);
	}
	file_count = dlist_size(file_list);

	req->data.total_files = file_count;
	req->data.file_idx = 0;

	dlist_for_each(file_list, node) {
		entry = node->data;
		DEBUG((DEBUG_DEBUG, "begin to send %s\n", entry->abs_path));
		snprintf(tmp, sizeof(tmp), "%s/%s", target, entry->rel_path);
		ret = send_file_by_put_req(fd, entry->abs_path, tmp, req_packet, req, resp_packet);
		if (ret == -1) {
			DEBUG((DEBUG_ERROR, "send file failed!|file=%s\n", entry->abs_path));
			break;
		}
		req->data.file_idx++;
	}

	dlist_destroy(file_list);

	DBUG_RETURN(ret);
}

int send_files_by_put_req(int fd, char *path, char *target,
		struct sftt_packet *req_packet, struct put_req *req,
		struct sftt_packet *resp_packet)
{
	DBUG_ENTER(__func__);

	int ret;

	if (is_file(path)) {
		req->data.total_files = 1;
		req->data.file_idx = 0;
		ret = send_file_by_put_req(fd, path, target, req_packet, req, resp_packet);
	} else {
		ret = send_dir_by_put_req(fd, path, target, req_packet, req, resp_packet);
	}

	DBUG_RETURN(ret);
}
