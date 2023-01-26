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
#include <errno.h>
#include <libgen.h>
#include <stdio.h>
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

extern struct mem_pool *g_mp;

int recv_file_from_get_resp(int fd, char *path, int type, unsigned int mode, struct sftt_packet *resp_packet)
{
	DBUG_ENTER(__func__);

	struct get_resp *resp;
	struct common_resp *com_resp;
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
		DEBUG((DEBUG_ERROR, "alloc com_resp failed!\n"));
		DBUG_RETURN(-1);
	}

	rp = path;

	/* Receive and handle file name, need reply */
	if (IS_DIR(type)) {
		DEBUG((DEBUG_INFO, "get a dir|dir=%s\n", rp));

		if (!file_existed(rp)) {
			ret = mkdirp(rp, mode);
			if (ret == -1) {
				DEBUG((DEBUG_WARN, "create dir failed|rp=%s\n", rp));
				send_common_resp(fd, resp_packet, com_resp, RESP_INTERNAL_ERR, 0);
				goto done;
			}
		}

		DEBUG((DEBUG_INFO, "recv dir done!|rp=%s\n", rp));
		send_common_resp(fd, resp_packet, com_resp, RESP_OK, 0);
		goto done;

	} else if (IS_FILE(type)) {
		DEBUG((DEBUG_INFO, "get a file|file=%s\n", rp));

		if (!file_existed(rp)) {
			ret = create_new_file_with_parent(rp, mode);
			if (ret == -1) {
				DEBUG((DEBUG_WARN, "create file failed|rp=%s\n", rp));
				send_common_resp(fd, resp_packet, com_resp, RESP_INTERNAL_ERR, 0);
				goto done;
			}
		}

		DEBUG((DEBUG_INFO, "recv file name done!|rp=%s\n", rp));
		send_common_resp(fd, resp_packet, com_resp, RESP_OK, 0);

	} else {
		DEBUG((DEBUG_WARN, "unknown file type|type=%d\n", type));
		send_common_resp(fd, resp_packet, com_resp, RESP_UNKNOWN_FILE_TYPE, 0);
		ret = -1;
		goto done;
	}
	/* Before to receive file content, the file should be existed */
	assert(file_existed(rp) && is_file(rp));

	DEBUG((DEBUG_INFO, "begin to recv file md5\n"));
	start = get_double_time();
	start_progress_viewer(&pv, 1000 * 1000);

	/* Receive file md5, need reply */
	ret = recv_sftt_packet(fd, resp_packet);
	if (ret == -1) {
		DEBUG((DEBUG_WARN, "recv file md5 packet failed!\n"));
		goto done;
	}
	resp = resp_packet->obj;
	total_size = resp->data.entry.total_size;

	strncpy(md5, (char *)resp->data.entry.content, MD5_STR_LEN);
	if (same_file(rp, md5)) {
		DEBUG((DEBUG_INFO, "file not changed|file=%s\n", rp));

		send_common_resp(fd, resp_packet, com_resp, RESP_OK, 0);

		DEBUG((DEBUG_INFO, "recv %s done!\n", rp));

		recv_size = total_size;
		now = get_double_time();
		speed = recv_size * 1.0 / (now - start);
		left_time = speed ? (total_size - recv_size) / speed : 0;

		format_trans_speed(speed, speed_info, sizeof(speed_info));
		format_trans_size(recv_size, recv_size_info, sizeof(recv_size_info));
		format_left_time(left_time, left_time_info, sizeof(left_time_info));
		progress = total_size ? (recv_size * 1.0) / total_size : 1.0;
		snprintf(progress_info, 128, "%s    %d%% %s %s %s", basename(rp),
				(int)(progress * 100), recv_size_info, speed_info,
				left_time_info);

		stop_progress_viewer(&pv, progress_info);
		mp_free(g_mp, resp);
		goto done;
	}
	mp_free(g_mp, resp);

	DEBUG((DEBUG_INFO, "send md5 common resp\n"));

	send_common_resp(fd, resp_packet, com_resp, RESP_CONTINUE, 0);

	fp = fopen(rp, "w+");
	if (fp == NULL) {
		DEBUG((DEBUG_WARN, "open file for write failed|file=%s\n", rp));
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
			DEBUG((DEBUG_WARN, "recv sftt packet failed\n"));
			break;
		}
		resp = resp_packet->obj;
		DEBUG((DEBUG_INFO, "received a block|len=%d\n", resp->data.entry.this_size));

		fwrite(resp->data.entry.content, resp->data.entry.this_size, 1, fp);

		if (resp->need_reply) {
			/* send response */
			DEBUG((DEBUG_INFO, "send file block common resp\n"));
			ret = send_common_resp(fd, resp_packet, com_resp, RESP_OK, 0);
			if (ret == -1) {
				DEBUG((DEBUG_ERROR, "send put response failed!\n"));
				break;
			}
		}

		recv_size += resp->data.entry.this_size;
		now = get_double_time();
		speed = recv_size * 1.0 / (now - start);
		left_time = speed ? (total_size - recv_size) / speed : 0;

		format_trans_speed(speed, speed_info, sizeof(speed_info));
		format_trans_size(recv_size, recv_size_info, sizeof(recv_size_info));
		format_left_time(left_time, left_time_info, sizeof(left_time_info));
		progress = total_size ? (recv_size * 1.0) / total_size : 1.0;
		snprintf(progress_info, 128, "%s    %d%% %s %s %s", basename(rp),
				(int)(progress * 100), recv_size_info, speed_info,
				left_time_info);

		show_progress(&pv, progress_info);

		if (recv_size == total_size)
			stop_progress_viewer(&pv, progress_info);

		mp_free(g_mp, resp);
	} while (recv_size < total_size);

	fclose(fp);

	if (recv_size < total_size) {
		DEBUG((DEBUG_WARN, "recv one file failed|rp=%s\n", rp));
		ret = -1;
		goto done;
	}

	if (!same_file(rp, md5)) {
		DEBUG((DEBUG_WARN, "recv one file failed for md5 error|rp=%s\n", rp));
		ret = -1;
		goto done;
	}

	set_file_mode(rp, mode);
	DEBUG((DEBUG_INFO, "recv file done|rp=%s\n", rp));

done:
	if (com_resp)
		mp_free(g_mp, com_resp);

	DBUG_RETURN(ret);
}

int recv_files_from_get_resp(int fd, char *path, struct sftt_packet *req_packet, struct sftt_packet *resp_packet)
{
	DBUG_ENTER(__func__);

	int ret = 0;
	char *rp = NULL;
	struct get_resp *resp = NULL;
	int recv_count = 0;
	unsigned int mode;
	int type;
	int total_files = 0;

	/* recv file name */
	ret = recv_sftt_packet(fd, resp_packet);
	if (ret == -1) {
		DEBUG((DEBUG_ERROR, "recv sftt packet failed!\n"));
		DBUG_RETURN(-1);
	}
	resp = resp_packet->obj;
	total_files = resp->data.total_files;

	DEBUG((DEBUG_INFO, "total files will recv|total_files=%d\n", total_files));

	if (!(total_files > 0)) {
		DEBUG((DEBUG_WARN, "target file not exist\n"));
		mp_free(g_mp, resp);
		DBUG_RETURN(-1);
	}

	if (total_files == 1 && IS_FILE(resp->data.entry.type)) {
		if (is_dir(path))
			rp = path_join(path, (char *)resp->data.entry.content);
		else
			rp = path;

		type = resp->data.entry.type;
		mode = resp->data.entry.mode;
		mp_free(g_mp, resp);
		DBUG_RETURN(recv_file_from_get_resp(fd, rp, type,
				mode, resp_packet));
	}
	/* Free resp in while loop below */
	//mp_free(g_mp, resp);

	if (!is_dir(path)) {
		DEBUG((DEBUG_ERROR, "when you recv dir or multi files you"
			       " should specify a dir|path=%s\n", path));
		DBUG_RETURN(-1);
	}

	recv_count = 0;
	do {
		DEBUG((DEBUG_INFO, "begin recv file|idx=%d\n", recv_count));

		rp = path_join(path, (char *)resp->data.entry.content);
		type = resp->data.entry.type;
		mode = resp->data.entry.mode;
		mp_free(g_mp, resp);
		ret = recv_file_from_get_resp(fd, rp, type, mode, resp_packet);
		if (ret == -1) {
			DEBUG((DEBUG_ERROR, "recv file failed|idx=%d\n", recv_count));
			DBUG_RETURN(-1);
		}

		DEBUG((DEBUG_INFO, "end recv file|idx=%d\n", recv_count));

		recv_count++;
		if (recv_count == total_files)
			break;

		/* recv file name */
		ret = recv_sftt_packet(fd, resp_packet);
		if (ret == -1) {
			DEBUG((DEBUG_ERROR, "recv sftt packet failed!\n"));
			DBUG_RETURN(-1);
		}

		resp = resp_packet->obj;
	} while (recv_count < total_files);

	DBUG_RETURN(0);
}

int recv_file_from_put_req(int fd, struct sftt_packet *req_packet,
	struct sftt_packet *resp_packet, struct put_resp *resp, bool *has_more)
{
	DBUG_ENTER(__func__);

	char *parent = NULL;
	char md5[MD5_STR_LEN];
	FILE *fp = NULL;
	int i = 0, ret = 0;
	int total_files = 0, file_idx = 0;
	int recv_size = 0, total_size = 0;
	struct put_req *req = NULL;
	char tmp[FILE_NAME_MAX_LEN];
	char rp[FILE_NAME_MAX_LEN];

	*has_more = true;

	req = (struct put_req *)req_packet->obj;
	assert(req != NULL);

	total_files = req->data.total_files;
	file_idx = req->data.file_idx;
	DEBUG((DEBUG_INFO, "total_files=%d|file_idx=%d\n",
		total_files, file_idx));

	strncpy(rp, (char *)req->data.entry.content, FILE_NAME_MAX_LEN - 1);
	DEBUG((DEBUG_INFO, "received put req|file=%s\n", rp));
	if (req->data.entry.type == FILE_TYPE_DIR) {
		if (!file_existed(rp)) {
			// FIXME: care about the ret
			ret = mkdirp(rp, req->data.entry.mode);
		}

		/* send resp */
		ret = send_put_resp(fd, resp_packet, resp, RESP_OK, REQ_RESP_FLAG_NONE);
		if (ret == -1) {
			printf("%s: send resp failed!\n", __func__);
			DBUG_RETURN(-1);
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
				DEBUG((DEBUG_ERROR, "create parent dir failed when recv file|parent=%s\n", parent));
				/* send resp */
				ret = send_put_resp(fd, resp_packet, resp, RESP_INTERNAL_ERR, REQ_RESP_FLAG_NONE);
				if (ret == -1) {
					DEBUG((DEBUG_WARN, "send resp failed!\n"));
				}

				DBUG_RETURN(-1);
			}

			/* create new file */
			ret = create_new_file_with_parent(rp, req->data.entry.mode);
			if (ret == -1) {
				DEBUG((DEBUG_WARN, "create file failed when recv file|file=%s|err=%s\n",
						rp, strerror(errno)));
				/* send resp */
				ret = send_put_resp(fd, resp_packet, resp, RESP_INTERNAL_ERR, REQ_RESP_FLAG_NONE);
				if (ret == -1) {
					DEBUG((DEBUG_WARN, "send resp failed!\n"));
				}

				DBUG_RETURN(-1);
			}
		}

		/* send resp */
		ret = send_put_resp(fd, resp_packet, resp, RESP_OK, REQ_RESP_FLAG_NONE);
		if (ret == -1) {
			DEBUG((DEBUG_WARN, "send resp failed!\n"));
			DBUG_RETURN(-1);
		}
	}
	mp_free(g_mp, req);

	/* recv md5 packet */
	DEBUG((DEBUG_INFO, "begin receive file md5 ...\n"));
	ret = recv_sftt_packet(fd, req_packet);
	if (ret == -1) {
		DEBUG((DEBUG_ERROR, "recv file md5 failed!|rep\n"));
		DBUG_RETURN(-1);
	}
	req = req_packet->obj;

	DEBUG((DEBUG_INFO, "file total size: %d\n", req->data.entry.total_size));

	/* save md5 */
	strncpy(md5, (char *)req->data.entry.content, MD5_STR_LEN);
	if (same_file(rp, md5)) {
		DEBUG((DEBUG_INFO, "file not changed: %s\n", rp));

		/* send resp */
		ret = send_put_resp(fd, resp_packet, resp, RESP_OK, REQ_RESP_FLAG_NONE);
		if (ret == -1) {
			DEBUG((DEBUG_WARN, "send md5 resp failed|\n"));
			DBUG_RETURN(-1);
		}

		goto recv_one_file_done;

	} else {
		/* send resp */
		ret = send_put_resp(fd, resp_packet, resp, RESP_CONTINUE, REQ_RESP_FLAG_NONE);
		if (ret == -1) {
			DEBUG((DEBUG_WARN, "send md5 resp failed!\n"));
			DBUG_RETURN(-1);
		}
	}

	total_size = req->data.entry.total_size;
	DEBUG((DEBUG_INFO, "begin receive file content ...|total_size=%d\n", total_size));
	fp = fopen(rp, "w+");
	if (fp == NULL) {
		strncpy(tmp, rp, FILE_NAME_MAX_LEN - 1);
		parent = dirname(tmp);
		if (!file_existed(parent)) {
			mkdirp(parent, req->data.entry.mode);
		}

		fp = fopen(rp, "w+");
		if (fp == NULL) {
			DEBUG((DEBUG_ERROR, "create file failed|rp=%s\n", rp));
			DBUG_RETURN(-1);
		}
	}
	mp_free(g_mp, req);

	i = 0;
	do {
		ret = recv_sftt_packet(fd, req_packet);
		if (!(ret > 0)) {
			DEBUG((DEBUG_ERROR, "recv file block failed!|rp=%s|idx=%d\n",
				rp, i));
			break;
		}
		req = req_packet->obj;
		DEBUG((DEBUG_INFO, "receive %d-th block file content|size=%d\n",
			(i + 1), req->data.entry.this_size));

		fwrite(req->data.entry.content, req->data.entry.this_size, 1, fp);

		if (req->need_reply) {
			/* send response */
			DEBUG((DEBUG_INFO, "send file block put resp\n"));
			ret = send_put_resp(fd, resp_packet, resp, RESP_OK, REQ_RESP_FLAG_NONE);
			if (ret == -1) {
				DEBUG((DEBUG_ERROR, "send put response failed!\n"));
				break;
			}
		}
		recv_size += req->data.entry.this_size;
		i += 1;
		mp_free(g_mp, req);
	} while (recv_size < total_size);

	fclose(fp);

	if (recv_size != total_size) {
		DEBUG((DEBUG_WARN, "recv one file failed for len error|rp=%s|recv_size=%d|"
					"total_size=%d\n", rp, recv_size, total_size));
		DBUG_RETURN(-1);
	}

	if (!same_file(rp, md5)) {
		DEBUG((DEBUG_WARN, "recv one file failed for md5 error|rp=%s", rp));
		DBUG_RETURN(-1);
	}

recv_one_file_done:
	if (file_idx == total_files - 1)
		*has_more = false;

	DEBUG((DEBUG_INFO, "recv done!|rp=%s\n", rp));

	DBUG_RETURN(ret);
}

int recv_files_from_put_req(int fd, struct sftt_packet *req_packet, struct sftt_packet *resp_packet)
{
	DBUG_ENTER(__func__);

	int ret, i;
	bool has_more;
	struct put_resp *resp = NULL;

	resp = mp_malloc(g_mp, __func__, sizeof(struct put_resp));
	if (resp == NULL) {
		DEBUG((DEBUG_ERROR, "alloc put_resp failed!\n"));
		DBUG_RETURN(-1);
	}

	i = 0;
	do {
		DEBUG((DEBUG_INFO, "recv %d-th file ...\n", i));
		ret = recv_file_from_put_req(fd, req_packet, resp_packet,
			resp, &has_more);
		if (ret == -1 || has_more == false)
			break;

		ret = recv_sftt_packet(fd, req_packet);
		if (ret == -1) {
			DEBUG((DEBUG_ERROR, "recv encountered unrecoverable error ...\n"));
			break;
		}
		++i;

	} while (has_more);

	if (resp)
		mp_free(g_mp, resp);

	DBUG_RETURN(ret);
}
