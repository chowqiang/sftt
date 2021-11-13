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

#define SIZE_1G	(1024 * 1024 * 1024)
#define SIZE_1M (1024 * 1024)
#define SIZE_1K 1024

extern struct mem_pool *g_mp;

int send_file_content_by_get_resp(int fd, struct sftt_packet *resp_packet,
	struct get_resp *resp, int next)
{
	resp->need_reply = 1;

	return send_get_resp(fd, resp_packet, resp, RESP_OK, next);
}

int send_file_name_by_get_resp(int fd, char *path, char *fname,
	struct sftt_packet *resp_packet, struct get_resp *resp, int next)
{
	struct get_resp_data *data;

	data = &resp->data;

	if (is_dir(path))
		data->entry.type = FILE_TYPE_DIR;
	else
		data->entry.type = FILE_TYPE_FILE;

	data->entry.mode = file_mode(path);
	strncpy((char *)data->entry.content, fname, FILE_NAME_MAX_LEN);
	data->entry.this_size = strlen(fname);

	resp->need_reply = 0;

	return send_get_resp(fd, resp_packet, resp, RESP_OK, next);
}

int send_file_md5_by_get_resp(int fd, char *path, struct sftt_packet *resp_packet,
	struct get_resp *resp)
{
	int ret;
	struct get_resp_data *data;

	if (is_dir(path))
		return 0;

	resp = resp_packet->obj;
	data = &resp->data;

	data->entry.total_size = file_size(path);

	ret = md5_file(path, data->entry.content);
	if (ret == -1)
		return -1;

	data->entry.this_size = strlen((char *)data->entry.content);

	DEBUG((DEBUG_INFO, "file=%s|md5=%s\n", path, data->entry.content));
	resp->need_reply = 1;

	return send_get_resp(fd, resp_packet, resp, RESP_OK, 1);
}

int send_file_by_get_resp(int fd, char *path, char *fname,
	struct sftt_packet *resp_packet, struct get_resp *resp)
{
	struct get_resp_data *data;
	struct common_resp *com_resp;
	int ret, is_last, next;
	long read_size;
	FILE *fp;

	is_last = resp->data.file_idx == resp->data.total_files - 1;

	if (is_dir(path)) {
		next = is_last ? 0 : 1;
		DEBUG((DEBUG_INFO, "send dir name|path=%s|fname=%s\n", path, fname));

		return send_file_name_by_get_resp(fd, path, fname, resp_packet, resp, next);
	}

	DEBUG((DEBUG_INFO, "send file name|path=%s|fname=%s\n", path, fname));

	ret = send_file_name_by_get_resp(fd, path, fname, resp_packet, resp, 1);
	if (ret == -1)
		return -1;

	DEBUG((DEBUG_INFO, "send file md5|path=%s|fname=%s\n", path, fname));

	ret = send_file_md5_by_get_resp(fd, path, resp_packet, resp);
	if (ret == -1)
		return -1;

	DEBUG((DEBUG_INFO, "recv md5 resp|path=%s|fname=%s\n", path, fname));

	ret = recv_sftt_packet(fd, resp_packet);
	if (ret == -1) {
		printf("%s: recv sftt packet failed!\n", __func__);
		return -1;
	}

	com_resp = resp_packet->obj;
	if (com_resp->status == RESP_OK) {
		DEBUG((DEBUG_INFO, "file not changed|path=%s|skip ...\n", path));
		return 0;
	}

	fp = fopen(path, "r");
	if (fp == NULL)
		return -1;

	data = &resp->data;
	data->entry.total_size = file_size(path);
	read_size = 0;

	DEBUG((DEBUG_INFO, "begin to send file block\n"));

	do {
		ret = fread(data->entry.content, 1, CONTENT_BLOCK_SIZE, fp);

		data->entry.this_size = ret;
		read_size += ret;

		next = is_last ? (read_size < data->entry.total_size ? 1 : 0) : 1;

		DEBUG((DEBUG_INFO, "send file block|len=%d|next=%d\n", ret, next));

		ret = send_file_content_by_get_resp(fd, resp_packet, resp, next);
		if (ret == -1) {
			printf("%s: recv sftt packet failed!\n", __func__);
			break;
		}

#if 1
		DEBUG((DEBUG_INFO, "recv common resp\n"));

		ret = recv_sftt_packet(fd, resp_packet);
		if (ret == -1) {
			printf("%s: recv sftt packet failed!\n", __func__);
			return -1;
		}

		com_resp = resp_packet->obj;
		if (com_resp->status != RESP_OK) {
			printf("recv response failed!\n");
			break;
		}
#endif
	} while (read_size < data->entry.total_size);

	if (!feof(fp))
		ret = -1;

	fclose(fp);

	return ret;
}

int send_dir_by_get_resp(int fd, char *path, struct sftt_packet *resp_packet,
	struct get_resp *resp)
{
	struct dlist *file_list;
	struct dlist_node *node;
	struct get_resp_data *data;
	int file_count;
	struct path_entry *entry;

	DEBUG((DEBUG_INFO, "send dir in ...|path: %s\n", path));

	file_list = get_path_entry_list(path, NULL);
	if (file_list == NULL) {
		printf("%s:%d, cannot get file list, please ensure the path"
				" is absolute path! (path: %s)\n",
				__func__, __LINE__, path);
		return -1;
	}

	file_count = dlist_size(file_list);
	DEBUG((DEBUG_INFO, "file list|file_count=%d\n", file_count));

	data = &resp->data;
	data->total_files = file_count;
	data->file_idx = 0;

	dlist_for_each(file_list, node) {
		entry = node->data;

		DEBUG((DEBUG_INFO, "send file|idx=%d|path=%s\n",
					data->file_idx, entry->abs_path));

		if (send_file_by_get_resp(fd, entry->abs_path, entry->rel_path,
				resp_packet, resp) == -1) {
			DEBUG((DEBUG_ERROR, "send file failed: %s\n",
					(char *)entry->abs_path));
		}

		DEBUG((DEBUG_INFO, "send file done|idx=%d|path=%s\n",
					data->file_idx, entry->abs_path));

		data->file_idx++;
	}

	DEBUG((DEBUG_INFO, "send dir out\n"));

	return 0;
}

int send_files_by_get_resp(int fd, char *path, struct sftt_packet *resp_packet,
		struct get_resp *resp)
{
	int ret;
	char *fname;
	struct get_resp_data *data;

	if (is_file(path)) {
		DEBUG((DEBUG_INFO, "send file|path=%s\n", path));

		data = &resp->data;
		data->total_files = 1;
		data->file_idx = 0;

		fname = get_basename(path);
		ret = send_file_by_get_resp(fd, path, fname, resp_packet, resp);

		DEBUG((DEBUG_INFO, "send file done|path=%s\n", path));

	} else {
		DEBUG((DEBUG_INFO, "send multi files|path=%s\n", path));

		ret = send_dir_by_get_resp(fd, path, resp_packet, resp);

		DEBUG((DEBUG_INFO, "send multi files done|path=%s\n", path));
	}

	return ret;
}

int send_trans_entry_by_put_req(int fd,
	struct sftt_packet *req_packet, struct put_req *req)
{
	req_packet->type = PACKET_TYPE_PUT_REQ;

	req_packet->obj = req;
	//req_packet->block_size = PUT_REQ_PACKET_MIN_LEN;

	int ret = send_sftt_packet(fd, req_packet);
	if (ret == -1) {
		printf("%s: send sftt packet failed!\n", __func__);
		return -1;
	}

	return 0;
}


int send_file_name_by_put_req(int fd, struct sftt_packet *req_packet,
		char *file, char *target, struct put_req *req)
{
	struct put_req_data *data;

	data = &req->data;
	if (is_dir(file))
		data->entry.type = FILE_TYPE_DIR;
	else
		data->entry.type = FILE_TYPE_FILE;

	data->entry.mode = file_mode(file);
	strncpy((char *)data->entry.content, target, FILE_NAME_MAX_LEN);
	data->entry.this_size = strlen(target);

	req->need_reply = 1;

	return send_trans_entry_by_put_req(fd, req_packet, req);
}

int send_file_md5_by_put_req(int fd, struct sftt_packet *req_packet, char *file,
	struct put_req *req)
{
	int ret;
	struct put_req_data *data;

	if (is_dir(file))
		return 0;

	data = &req->data;
	//data->entry.total_size = file_size(file);

	ret = md5_file(file, data->entry.content);
	if (ret == -1)
		return -1;

	data->entry.this_size = strlen((char *)data->entry.content);

	req->need_reply = 1;

	return send_trans_entry_by_put_req(fd, req_packet, req);
}

int send_file_content_by_put_req(int fd,
	struct sftt_packet *req_packet, struct put_req *req)
{
	req->need_reply = 1;

	return send_trans_entry_by_put_req(fd, req_packet, req);
}

void format_trans_speed(long speed, char *buf, int max_len)
{
	if (speed > SIZE_1G) {
		snprintf(buf, max_len, "%.2fGB/s", speed * 1.0 / SIZE_1G);
	} else if (speed > SIZE_1M) {
		snprintf(buf, max_len, "%.2fMB/s", speed * 1.0 / SIZE_1M);
	} else if (speed > SIZE_1K) {
		snprintf(buf, max_len, "%.2fKB/s", speed * 1.0 / SIZE_1K);
	} else {
		snprintf(buf, max_len, "%dB/s", (int)speed);
	}
}

void format_left_time(int left, char *buf, int max_len)
{
	int hour, minute, second;

	if (left > 3600) {
		hour = left / 3600;
		minute = left % 3600 / 60;
		second = left % 60;
		if (hour >= 10)
			snprintf(buf, max_len, "%d:%02d:%02d", hour, minute,
					second);
		else
			snprintf(buf, max_len, "%02d:%02d:%02d", hour, minute,
					second);
	} else {
		minute = left / 60;
		second = left % 60;
		snprintf(buf, max_len, "%02d:%02d", minute, second);
	}
}

void format_trans_size(long size, char *buf, int max_len)
{
	if (size > SIZE_1G) {
		snprintf(buf, max_len, "%.2fGB", size * 1.0 / SIZE_1G);
	} else if (size > SIZE_1M) {
		snprintf(buf, max_len, "%.2fMB", size * 1.0 / SIZE_1M);
	} else if (size > SIZE_1K) {
		snprintf(buf, max_len, "%.2fKB", size * 1.0 / SIZE_1K);
	} else {
		snprintf(buf, max_len, "%dB", (int)size);
	}
}

int send_file_by_put_req(int fd, char *file, char *target, struct sftt_packet *req_packet,
		struct put_req *req, struct sftt_packet *resp_packet)
{
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
	
	// if it is the last file?
	is_last = req->data.file_idx == (req->data.total_files - 1);

	req->next = is_last && is_dir(file) ? 0 : 1;

	// send file name
	ret = send_file_name_by_put_req(fd, req_packet, file, target, req);
	if (ret == -1) {
		printf("%s:%d, send file name failed!\n", __func__, __LINE__);
		return -1;
	}

	// recv response for creating file
	ret = recv_sftt_packet(fd, resp_packet);
	if (ret == -1) {
		printf("%s:%d, recv sftt packet failed!\n", __func__, __LINE__);
		return -1;
	}
	resp = resp_packet->obj;
	if (resp->status != RESP_OK) {
		printf("%s:%d, server create file failed!\n", __func__, __LINE__);
		return -1;
	}

	// done when file is dir
	if (is_dir(file))
		return 0;

	start = get_double_time();
	start_progress_viewer(&pv, 1000 * 1000);

	total_size = file_size(file);
	// send md5 of file
	ret = send_file_md5_by_put_req(fd, req_packet, file, req);
	if (ret == -1) {
		printf("%s:%d, send md5 failed!\n", __func__, __LINE__);
		return -1;
	}

	// recv response for checking md5 of file
	ret = recv_sftt_packet(fd, resp_packet);
	if (ret == -1) {
		printf("%s: recv sftt packet failed!\n", __func__);
		return -1;
	}
	resp = resp_packet->obj;
	if (resp->status == RESP_OK) {
		DEBUG((DEBUG_DEBUG, "file not changed: %s, skip ...\n", file));

		send_size = total_size;
		progress = send_size / total_size;
		now = get_double_time();
		speed = send_size * 1.0 / (now - start);
		left_time = (total_size - send_size) / speed;

		format_trans_speed(speed, speed_info, sizeof(speed_info));
		format_trans_size(send_size, send_size_info, sizeof(send_size_info));
		format_left_time(left_time, left_time_info, sizeof(left_time_info));
		snprintf(progress_info, 128, "%s    %d%% %s %s %s", file,
				(int)(progress * 100), send_size_info, speed_info, left_time_info);

		stop_progress_viewer(&pv, progress_info);

		return 0;
	}

	DEBUG((DEBUG_DEBUG, "open file: %s\n", file));
	// open file for reading
	fp = fopen(file, "r");
	if (fp == NULL) {
		printf("open file failed: %s\n", file);
		return -1;
	}

	req->data.entry.total_size = total_size;

	while (!feof(fp)) {
		ret = fread(req->data.entry.content, 1, CONTENT_BLOCK_SIZE, fp);
		req->data.entry.this_size = ret;
		req->next = is_last && feof(fp) ? 0 : 1;
		ret = send_file_content_by_put_req(fd, req_packet, req);
		if (ret == -1) {
			break;
		}

#if 1
		ret = recv_sftt_packet(fd, resp_packet);
		if (ret == -1) {
			printf("%s: recv sftt packet failed!\n", __func__);
			break;
		}
		resp = resp_packet->obj;
		if (resp->status != RESP_OK) {
			printf("recv response failed!\n");
			break;
		}
#endif
		send_size += req->data.entry.this_size;
		progress = send_size / total_size;
		now = get_double_time();
		speed = send_size * 1.0 / (now - start);
		left_time = (total_size - send_size) / speed;

		format_trans_speed(speed, speed_info, sizeof(speed_info));
		format_trans_size(send_size, send_size_info, sizeof(send_size_info));
		format_left_time(left_time, left_time_info, sizeof(left_time_info));
		snprintf(progress_info, 128, "%s    %d%% %s %s %s", file,
				(int)(progress * 100), send_size_info, speed_info,
				left_time_info);
		show_progress(&pv, progress_info);

		if (send_size == total_size)
			stop_progress_viewer(&pv, progress_info);

	}

	if (!feof(fp)) {
		printf("%s:%d, read file failed!\n", __func__, __LINE__);
		ret = -1;
	}

	fclose(fp);

	return ret;
}

int send_dir_by_put_req(int fd, char *path, char *target,
	struct sftt_packet *req_packet, struct put_req *req,
	struct sftt_packet *resp_packet)
{
	int ret, file_count;
	char tmp[FILE_NAME_MAX_LEN + 2];
	struct path_entry *entry;
	struct dlist *file_list;
	struct dlist_node *node;

	file_list = get_path_entry_list(path, NULL);
	if (file_list == NULL) {
		DEBUG((DEBUG_ERROR, "file list is null!\n"));
		return -1;
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
			printf("send file failed: %s\n", entry->abs_path);
			break;
		}
		req->data.file_idx++;
	}

	return ret;
}

int send_files_by_put_req(int fd, char *path, char *target,
		struct sftt_packet *req_packet, struct put_req *req)
{
	struct sftt_packet *resp_packet;
	int ret;
	
	resp_packet = malloc_sftt_packet();
	if (resp_packet == NULL) {
		printf("%s:%d, alloc sftt packet failed!\n", __func__, __LINE__);
		return -1;
	}

	if (is_file(path)) {
		req->data.total_files = 1;
		req->data.file_idx = 0;
		ret = send_file_by_put_req(fd, path, target, req_packet, req, resp_packet);
	} else {
		ret = send_dir_by_put_req(fd, path, target, req_packet, req, resp_packet);
	}

	return ret;
}

int recv_file_from_get_resp(int fd, char *path, int type, u_long mode, struct sftt_packet *resp_packet)
{
	struct get_resp *resp;
	struct get_resp_data *data;
	struct common_resp *com_resp;
	struct common_resp_data *com_data;
	char md5[MD5_STR_LEN];
	FILE *fp;
	char *rp;
	long total_size = 0;
	int ret = 0;
	struct progress_viewer pv;
	char progress_info[128];
	long recv_size = 0;
	long speed = 0;
	float progress;
	double start, now;
	int left_time;
	char speed_info[16];
	char left_time_info[16];
	char recv_size_info[16];

	com_resp = (struct common_resp *)mp_malloc(g_mp, "send_one_file_com_resp",
			sizeof(struct common_resp));
	if (com_resp == NULL) {
		printf("alloc com_resp failed!\n");
		return -1;
	}

	rp = path;

	if (IS_DIR(type)) {
		DEBUG((DEBUG_INFO, "get a dir|dir=%s\n", rp));

		if (!file_existed(rp)) {
			ret = mkdirp(rp, mode);
			if (ret == -1) {
				printf("create dir failed: %s\n", rp);
				send_common_resp(fd, resp_packet, com_resp, RESP_INTERNAL_ERR, 0);
				goto done;
			}
		}

		DEBUG((DEBUG_INFO, "recv %s done!\n", rp));

		goto done;
	} else if (IS_FILE(type)) {
		DEBUG((DEBUG_INFO, "get a file|file=%s\n", rp));

		if (!file_existed(rp)) {
			ret = create_new_file(rp, mode);
			if (ret == -1) {
				printf("create file failed: %s\n", rp);
				send_common_resp(fd, resp_packet, com_resp, RESP_INTERNAL_ERR, 0);
				goto done;
			}
		}
	} else {
		printf("unknown file type!\n");
		send_common_resp(fd, resp_packet, com_resp, RESP_UNKNOWN_FILE_TYPE, 0);
		ret = -1;
		goto done;
	}

	assert(file_existed(rp) && is_file(rp));	

	DEBUG((DEBUG_INFO, "begin to recv file md5\n"));
	start = get_double_time();
	start_progress_viewer(&pv, 1000 * 1000);

	/* recv md5 */
	ret = recv_sftt_packet(fd, resp_packet);
	if (ret == -1) {
		printf("%s: recv sftt packet failed!\n", __func__);
		goto done;
	}
	resp = resp_packet->obj;
	data = &resp->data;
	total_size = data->entry.total_size;

	strncpy(md5, (char *)data->entry.content, MD5_STR_LEN);
	if (same_file(rp, md5)) {
		DEBUG((DEBUG_INFO, "file not changed|file=%s\n", rp));

		send_common_resp(fd, resp_packet, com_resp, RESP_OK, 0);
		mp_free(g_mp, com_resp);

		DEBUG((DEBUG_INFO, "recv %s done!\n", rp));

		recv_size = total_size;
		progress = recv_size / total_size;
		now = get_double_time();
		speed = recv_size * 1.0 / (now - start);
		left_time = (total_size - recv_size) / speed;

		format_trans_speed(speed, speed_info, sizeof(speed_info));
		format_trans_size(recv_size, recv_size_info, sizeof(recv_size_info));
		format_left_time(left_time, left_time_info, sizeof(left_time_info));
		snprintf(progress_info, 128, "%s    %d%% %s %s %s", rp,
				(int)(progress * 100), recv_size_info, speed_info,
				left_time_info);

		stop_progress_viewer(&pv, progress_info);

		goto done;
	}

	DEBUG((DEBUG_INFO, "send md5 common resp\n"));

	send_common_resp(fd, resp_packet, com_resp, RESP_CONTINUE, 0);

	fp = fopen(rp, "w+");
	if (fp == NULL) {
		printf("%s: open file for write failed! file: %s\n", __func__, rp);
		send_common_resp(fd, resp_packet, com_resp, RESP_INTERNAL_ERR, 0);
		ret = -1;
		goto done;
	}

	DEBUG((DEBUG_INFO, "begin to recv file content|total_size=%ld\n",
				total_size));

	do {
		/* recv content */
		ret = recv_sftt_packet(fd, resp_packet);
		if (ret == -1) {
			printf("%s: recv sftt packet failed!\n", __func__);
			break;
		}
		resp = resp_packet->obj;
		data = &resp->data;
		DEBUG((DEBUG_INFO, "received a block|len=%d\n", data->entry.this_size));

		fwrite(data->entry.content, data->entry.this_size, 1, fp);

		DEBUG((DEBUG_INFO, "send file block common resp\n"));

		send_common_resp(fd, resp_packet, com_resp, RESP_OK, 0);

		recv_size += data->entry.this_size;
		progress = recv_size / total_size;
		now = get_double_time();
		speed = recv_size * 1.0 / (now - start);
		left_time = (total_size - recv_size) / speed;

		format_trans_speed(speed, speed_info, sizeof(speed_info));
		format_trans_size(recv_size, recv_size_info, sizeof(recv_size_info));
		format_left_time(left_time, left_time_info, sizeof(left_time_info));
		snprintf(progress_info, 128, "%s    %d%% %s %s %s", rp,
				(int)(progress * 100), recv_size_info, speed_info,
				left_time_info);

		show_progress(&pv, progress_info);

		if (recv_size == total_size)
			stop_progress_viewer(&pv, progress_info);

	} while (recv_size < total_size);

	fclose(fp);

	if (recv_size < total_size) {
		printf("%s: recv one file failed: %s\n", __func__, rp);
		ret = -1;
		goto done;
	}

	if (!same_file(rp, md5)) {
		printf("%s: recv one file failed: %s, "
			"md5 not correct!\n", __func__, rp);
		ret = -1;
		goto done;
	}

	set_file_mode(rp, data->entry.mode);
	DEBUG((DEBUG_INFO, "recv %s done\n", rp));

done:
	if (com_resp)
		mp_free(g_mp, com_resp);

	return ret;
}

int recv_files_from_get_resp(int fd, char *path, struct sftt_packet *resp_packet)
{
	int ret = 0;
	char *rp = NULL;
	struct get_resp *resp = NULL;
	struct get_resp_data *data = NULL;
	int recv_count = 0;

	/* recv file name */
	ret = recv_sftt_packet(fd, resp_packet);
	if (ret == -1) {
		printf("%s: recv sftt packet failed!\n", __func__);
		return -1;
	}
	resp = resp_packet->obj;
	data = &resp->data;

	DEBUG((DEBUG_INFO, "total files will recv: %d\n", data->total_files));

	if (!(data->total_files > 0)) {
		printf("%s: target file not exist\n", __func__);
		return -1;
	}

	if (data->total_files == 1 && IS_FILE(data->entry.type)) {
		if (is_dir(path))
			rp = path_join(path, (char *)data->entry.content);		
		else
			rp = path;

		return recv_file_from_get_resp(fd, rp, data->entry.type,
				data->entry.mode, resp_packet);
	}

	if (!is_dir(path)) {
		printf("%s:%d, when you recv dir or multi files you"
			       " should specify a dir (path: %s)\n",
			       __func__, __LINE__, path);
		return -1;
	}

	recv_count = 0;
	do {
		DEBUG((DEBUG_INFO, "begin recv %d-th file\n", recv_count));

		rp = path_join(path, (char *)data->entry.content);
		recv_file_from_get_resp(fd, rp, data->entry.type,
				data->entry.mode, resp_packet);	

		DEBUG((DEBUG_INFO, "end recv %d-th file\n", recv_count));

		recv_count++;
		if (recv_count == data->total_files)
			break;

		/* recv file name */
		ret = recv_sftt_packet(fd, resp_packet);
		if (ret == -1) {
			printf("%s: recv sftt packet failed!\n", __func__);
			return -1;
		}

		resp = resp_packet->obj;
		data = &resp->data;
	} while (recv_count < data->total_files);

	return 0;
}

int recv_file_by_put_req(int fd, struct sftt_packet *req_packet,
	struct sftt_packet *resp_packet, struct put_resp *resp, bool *has_more)
{
	char *rp = NULL, *parent = NULL;
	char md5[MD5_STR_LEN];
	FILE *fp = NULL;
	int i = 0, ret = 0, total_size = 0;
	struct put_req *req = NULL;
	char tmp[FILE_NAME_MAX_LEN];

	*has_more = true;

	req = (struct put_req *)req_packet->obj;
	assert(req != NULL);

	DEBUG((DEBUG_INFO, "file_name=%s\n", (char *)req->data.entry.content));
	DEBUG((DEBUG_INFO, "req_data->total_files=%d|req_data->file_idx=%d\n",
		req->data.total_files, req->data.file_idx));

	rp = (char *)req->data.entry.content;
	DEBUG((DEBUG_INFO, "received put req|file=%s\n", rp));
	if (req->data.entry.type == FILE_TYPE_DIR) {
		if (!file_existed(rp)) {
			ret = mkdirp(rp, req->data.entry.mode);
		}

		/* send resp */
		resp->status = RESP_OK;
		resp_packet->obj = resp;
		resp_packet->type = PACKET_TYPE_PUT_RESP;

		ret = send_sftt_packet(fd, resp_packet);
		if (ret == -1) {
			printf("%s: send resp failed!\n", __func__);
			return -1;
		}

		goto recv_one_file_done;
	} else {
		if (!file_existed(rp)) {
			ret = 0;
			/* check parent dir whether existed */
			strncpy(tmp, rp, FILE_NAME_MAX_LEN - 1);
			parent = dirname(tmp);
			if (!file_existed(parent)) {
				ret = mkdirp(parent, req->data.entry.mode);
			}
			if (ret == -1) {
				printf("%s:%d, create parent dir failed when recv file\n",
						__func__, __LINE__);
				/* send resp */
				resp->status = RESP_INTERNAL_ERR;
				resp_packet->obj = resp;
				resp_packet->type = PACKET_TYPE_PUT_RESP;

				ret = send_sftt_packet(fd, resp_packet);
				if (ret == -1) {
					printf("%s: send resp failed!\n", __func__);
				}

				return -1;
			}

			/* create new file */
			ret = create_new_file(rp, req->data.entry.mode);
			if (ret == -1) {
				perror("create_new_file failed");
				printf("%s:%d, create file failed when recv file, file=%s\n",
						__func__, __LINE__, rp);
				/* send resp */
				resp->status = RESP_INTERNAL_ERR;
				resp_packet->obj = resp;
				resp_packet->type = PACKET_TYPE_PUT_RESP;

				ret = send_sftt_packet(fd, resp_packet);
				if (ret == -1) {
					printf("%s: send resp failed!\n", __func__);
				}

				return -1;
			}
		}

		/* send resp */
		resp->status = RESP_OK;
		resp_packet->obj = resp;
		resp_packet->type = PACKET_TYPE_PUT_RESP;

		ret = send_sftt_packet(fd, resp_packet);
		if (ret == -1) {
			printf("%s: send resp failed!\n", __func__);
			return -1;
		}
	}

	/* recv md5 packet */
	DEBUG((DEBUG_INFO, "begin receive file md5 ...\n"));
	ret = recv_sftt_packet(fd, req_packet);
	if (ret == -1) {
		printf("recv encountered unrecoverable error ...\n");
		return -1;
	}
	req = req_packet->obj;

	DEBUG((DEBUG_INFO, "file total size: %ld\n", req->data.entry.total_size));

	/* save md5 */
	strncpy(md5, (char *)req->data.entry.content, MD5_STR_LEN);
	if (same_file(rp, md5)) {
		DEBUG((DEBUG_INFO, "file not changed: %s\n", rp));

		/* send resp */
		resp->status = RESP_OK;
		resp_packet->obj = resp;
		resp_packet->type = PACKET_TYPE_PUT_RESP;

		ret = send_sftt_packet(fd, resp_packet);
		if (ret == -1) {
			printf("%s: send resp failed!\n", __func__);
			return -1;
		}

		goto recv_one_file_done;

	} else {
		/* send resp */
		resp->status = RESP_CONTINUE;
		resp_packet->obj = resp;
		resp_packet->type = PACKET_TYPE_PUT_RESP;

		ret = send_sftt_packet(fd, resp_packet);
		if (ret == -1) {
			printf("%s: send resp failed!\n", __func__);
			return -1;
		}
	}

	DEBUG((DEBUG_INFO, "begin receive file content ...\n"));

	fp = fopen(rp, "w+");
	if (fp == NULL) {
		strncpy(tmp, rp, FILE_NAME_MAX_LEN - 1);
		parent = dirname(tmp);
		if (!file_existed(parent)) {
			mkdirp(parent, req->data.entry.mode);
		}
		
		fp = fopen(rp, "w+");
		if (fp == NULL) {
			printf("create file failed: %s\n", rp);
			return -1;
		}
	}

	i = 0;
	do {
		ret = recv_sftt_packet(fd, req_packet);
		if (!(ret > 0)) {
			printf("recv encountered unrecoverable error ...\n");
			break;
		}
		req = req_packet->obj;
		DEBUG((DEBUG_INFO, "receive %d-th block file content|size=%d\n",
			(i + 1), req->data.entry.this_size));

		fwrite(req->data.entry.content, req->data.entry.this_size, 1, fp);

		/* send response */
		resp->status = RESP_OK;
		resp_packet->obj = resp;
		resp_packet->type = PACKET_TYPE_PUT_RESP;

		ret = send_sftt_packet(fd, resp_packet);
		if (ret == -1) {
			printf("send put response failed!\n");
			break;
		}

		total_size += req->data.entry.this_size;
		i += 1;
	} while (total_size < req->data.entry.total_size);

	fclose(fp);

	if (total_size == req->data.entry.total_size) {
		DEBUG((DEBUG_INFO, "received one file: %s\n", rp));
		return 0;
	}

	if (!same_file(rp, md5)) {
		DEBUG((DEBUG_INFO, "recv one file failed: %s|"
			"md5 not correct!\n", rp));
		return -1;
	}

recv_one_file_done:
	if (req->data.file_idx == req->data.total_files - 1)
		*has_more = false;

	set_file_mode(rp, req->data.entry.mode);

	DEBUG((DEBUG_INFO, "recv %s done!\n", rp));

	return 0;
}

int recv_files_by_put_req(int fd, struct sftt_packet *req_packet)
{
	int ret, i;
	bool has_more;
	struct put_resp *resp = NULL;
	struct sftt_packet *resp_packet = NULL;

	resp = mp_malloc(g_mp, __func__, sizeof(struct put_resp));
	if (resp == NULL) {
		printf("%s:%d, alloc put_resp failed!\n", __func__, __LINE__);
		return -1;
	}

	resp_packet = malloc_sftt_packet();
	if (resp_packet == NULL) {
		printf("%s:%d, alloc sftt packet failed!\n", __func__, __LINE__);
		ret = -1;
		goto done;
	}

	i = 0;
	do {
		DEBUG((DEBUG_INFO, "recv %d-th file ...\n", i));
		ret = recv_file_by_put_req(fd, req_packet, resp_packet,
			resp, &has_more);
		if (ret == -1 || has_more == false)
			break;

		ret = recv_sftt_packet(fd, req_packet);
		if (ret == -1) {
			printf("recv encountered unrecoverable error ...\n");
			break;
		}
		++i;

	} while (has_more);

done:
	if (resp)
		mp_free(g_mp, resp);

	if (resp_packet)
		free_sftt_packet(&resp_packet);

	return ret;
}
