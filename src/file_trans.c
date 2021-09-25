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

#include "file_trans.h"

int send_file_content_by_get_resp(int fd, struct sftt_packet *resp_packet,
	struct get_resp *resp, int next)
{
	return send_get_resp(fd, resp_packet, resp, RESP_OK, next);
}

int send_file_name_by_get_resp(int fd, char *path, char *fname,
	struct sftt_packet *resp_packet, struct get_resp *resp, int next)
{
	struct get_resp_data *data;

	data = &resp->data;

	DEBUG((DEBUG_INFO, "path=%s|fname=%s\n", path, fname));

	if (is_dir(path))
		data->entry.type = FILE_TYPE_DIR;
	else
		data->entry.type = FILE_TYPE_FILE;

	data->entry.mode = file_mode(path);
	strncpy((char *)data->entry.content, fname, FILE_NAME_MAX_LEN);
	data->entry.this_size = strlen(fname);

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

	return send_get_resp(fd, resp_packet, resp, RESP_OK, 1);
}

int send_file_by_get_resp(int fd, char *path, char *fname,
	struct sftt_packet *resp_pacekt, struct get_resp *resp, int next)
{
	struct get_resp_data *data;
	struct common_resp *com_resp;
	int ret, next;
	long read_size;
	FILE *fp;

	if (is_dir(path))
		return send_file_name_by_get_resp(fd, path, fname, resp_packet, resp, next);

	ret = send_file_name_by_get_resp(fd, path, fname, resp_packet, resp, 1);
	if (ret == -1)
		return -1;

	ret = send_file_md5_by_get_resp(fd, path, resp_packet, resp);
	if (ret == -1)
		return -1;

	ret = recv_sftt_packet(client->connect_fd, resp_packet);
	if (ret == -1) {
		printf("%s: recv sftt packet failed!\n", __func__);
		return -1;
	}
	com_resp = resp_packet->obj;
	if (com_resp->status == RESP_OK) {
		DEBUG((DEBUG_INFO, "file not changed: %s|skip ...\n", path));
		return 0;
	}

	fp = fopen(path, "r");
	if (fp == NULL)
		return -1;

	data = &resp->data;
	data->entry.total_size = file_size(path);
	read_size = 0;
	do {
		ret = fread(data->entry.content, 1, CONTENT_BLOCK_SIZE, fp);
		DEBUG((DEBUG_INFO, "send file block|len=%d\n", ret));

		data->entry.this_size = ret;
		read_size += ret;

		next = read_size < data->entry.total_size ? 1 : 0;
		ret = send_file_content_by_get_resp(fd, resp_packet, resp, next);
		if (ret == -1) {
			printf("%s: recv sftt packet failed!\n", __func__);
			break;
		}

		com_resp = resp_packet->obj;
		if (com_resp->status != RESP_OK) {
			printf("recv response failed!\n");
			break;
		}

	} while (read_size < data->entry.total_size);

	if (!feof(fp))
		ret = -1;

	fclose(fp);

	return ret;
}

int send_dir_by_get_resp(int fd, char *path, struct sftt_packet *resp_packet,
	struct get_resp *resp)
{
	struct dlist *flist_list;
	struct dlist_node *node;
	struct get_resp_data *data;
	int i, file_count, next;
	struct path_entry *entry;

	DEBUG((DEBUG_INFO, "send dir: %s\n", entry->abs_path));

	file_list = get_path_entry_list(path, NULL);
	if (file_list) {
		printf("%s:%d, cannot get file list, please ensure the path"
				" is absolute path! (path: %s)\n",
				__func__, __LINE__, path);
		return -1;
	}

	file_count = dlist_size(file_list);
	DEBUG((DEBUG_INFO, "file_count: %d\n", file_count));

	data = &resp->data;
	data->total_files = file_count;
	data->file_idx = 0;
	dlist_for_each(file_list, node) {
		next = data->file_idx < file_count - 1;

		entry = node->data;
		DEBUG((DEBUG_INFO, "send %d-th file: %s\n", i, entry->abs_path));
		if (send_file_by_get_resp(fd, entry->abs_path, entry->rel_path,
				resp_packet, resp, next) == -1) {
			DEBUG((DEBUG_INFO, "send file failed: %s\n", (char *)node->data));
		}
		DEBUG((DEBUG_INFO, "send file done\n"));

		data->file_idx++;
	}
	DEBUG((DEBUG_INFO, "send dir done\n"));
}

int send_file_by_put_req(int fd, char *path, struct sftt_packet *req_packet)
{
	struct put_req *req;
	struct put_req_data *req_data;
	struct put_resp *resp;
	struct common_resp *com_resp;
	int ret;
	int len;
	int i = 0;
	FILE *fp;

	req = (struct put_req *)mp_malloc(g_mp, "send_one_file_req", sizeof(struct put_req));
	if (req == NULL)
		return -1;
	req_data = &req->data;

	strncpy(req->session_id, client->session_id, SESSION_ID_LEN);
	req_data->total_files = nr;
	req_data->file_idx = idx;
	//req_data->entry.idx = 0;

	com_resp = (struct common_resp *)mp_malloc(g_mp, "send_one_file_com_resp",
			sizeof(struct common_resp));
	assert(com_resp != NULL);

	if (is_dir(path))
		return send_file_name_by_put_req(client, req_packet, path, fname, req);

	ret = send_file_name_by_put_req(client, req_packet, path, fname, req);
	if (ret == -1)
		return -1;
	//req->entry.idx += 1;

	ret = send_file_md5_by_put_req(client, req_packet, path, req);
	if (ret == -1)
		return -1;
	//req->entry.idx += 1;

	ret = recv_sftt_packet(client->conn_ctrl.sock, resp_packet);
	if (ret == -1) {
		printf("%s: recv sftt packet failed!\n", __func__);
		return -1;
	}
	com_resp = resp_packet->obj;
	if (com_resp->status == RESP_OK) {
		printf("file not changed: %s, skip ...\n", path);
		return 0;
	}

	printf("open file: %s\n", path);
	fp = fopen(path, "r");
	if (fp == NULL) {
		printf("open file failed: %s\n", path);
		return -1;
	}

	while (!feof(fp)) {
		ret = fread(req_data->entry.content, 1, CONTENT_BLOCK_SIZE, fp);
		//printf("read block size: %d\n", ret);

		req_data->entry.this_size = ret;
		ret = send_file_content_by_put_req(client, req_packet, req);
		if (ret == -1) {
			break;
		}

		ret = recv_sftt_packet(client->conn_ctrl.sock, resp_packet);
		if (ret == -1) {
			printf("%s: recv sftt packet failed!\n", __func__);
			break;
		}

		resp = resp_packet->obj;
		if (resp->status != RESP_OK) {
			printf("recv response failed!\n");
			break;
		}

		//req->entry.idx += 1;
	}

	if (!feof(fp))
		ret = -1;

	fclose(fp);

	mp_free(g_mp, req);
	mp_free(g_mp, com_resp);

	return ret;

}

int send_dir_by_put_req(int fd, char *path, struct sftt_packet *req_packet)
{
	file_list = get_path_entry_list(file, NULL);
	file_count = dlist_size(file_list);
	dlist_for_each(file_list, node) {
		entry = node->data;
		printf("begin to send %s\n", entry->abs_path);
		if (send_file_by_put_req(client, req_packet, resp_packet,
			entry->abs_path, entry->rel_path, file_count, i) == -1) {
			printf("send file failed: %s\n", (char *)node->data);
		}
		++i;
	}

}

int recv_file_from_get_resp(int fd, char *path, struct sftt_packet *resp_packet) 
{
	struct get_resp *resp;
	struct get_resp_data *data;
	struct common_resp *com_resp;
	struct common_resp_data *com_data;
	u_long mode;
	int type;
	FILE *fp;

	resp = resp_packet->obj;
	data = &resp->data;
	type = data->entry.type;
	mode = data->entry.mode;
	mp_free(g_mp, resp);

	com_resp = (struct common_resp *)mp_malloc(g_mp, "send_one_file_com_resp",
			sizeof(struct common_resp));
	assert(com_resp != NULL);

	printf("get one file: %s\n", path);
	if (IS_DIR(type)) {
		if (!file_existed(rp)) {
			ret = mkdirp(rp, mode);
			if (ret == -1) {
				printf("create dir failed: %s\n", rp);
				send_common_resp(fd, resp_packet, com_resp, RESP_INTERNAL_ERR, 0);
				mp_free(g_mp, com_resp);
				return -1;
			}
		}
		send_common_resp(fd, resp_packet, com_resp, RESP_OK, 0);
		mp_free(g_mp, com_resp);
		return 0;
	}

	if (!file_existed(path)) {
		fp = fopen(rp, "w+");
		if (fp == NULL) {
			printf("create file failed: %s\n", rp);
			send_common_resp(fd, resp_packet, com_resp, RESP_INTERNAL_ERR, 0);
			mp_free(g_mp, com_rsp);
			return -1;
		}
		fclose(fp);
	}

	asssert(file_existed(path) && is_file(path));	

	/* recv md5 */
	ret = recv_sftt_packet(fd, resp_packet);
	if (ret == -1) {
		printf("%s: recv sftt packet failed!\n", __func__);
		return -1;
	}
	resp = resp_packet->obj;
	data = &resp->data;

	strncpy(md5, (char *)data->entry.content, MD5_STR_LEN);
	if (same_file(rp, md5)) {
		printf("file not changed: %s\n", rp);
		send_common_resp(fd, resp_packet, com_resp, RESP_OK, 0);
		mp_free(g_mp, com_rsp);
		return 0;
	} else {
		send_common_resp(fd, resp_packet, com_resp, RESP_CONTINUE, 0);
	}

	fp = fopen(rp, "w+");
	if (fp == NULL) {
		printf("%s: open file for write failed!\n", __func__);
		printf("file: %s\n", rp);
		send_common_resp(fd, resp_packet, com_resp, RESP_INTERNAL_ERR, 0);
		mp_free(g_mp, com_rsp);
		return -1;
	}

	do {
		/* recv content */
		ret = recv_sftt_packet(client->conn_ctrl.sock, resp_packet);
		if (ret == -1) {
			printf("%s: recv sftt packet failed!\n", __func__);
			break;
		}
		resp = resp_packet->obj;
		data = &resp->data;
		//printf("receive block len: %d\n", resp->entry.len);

		fwrite(data->entry.content, data->entry.this_size, 1, fp);

		/* send resp */
		com_resp->status = RESP_OK;
		resp_packet->obj = com_resp;
		resp_packet->type = PACKET_TYPE_COMMON_RESP;

		ret = send_sftt_packet(client->conn_ctrl.sock, resp_packet);
		if (ret == -1) {
			printf("%s: send resp failed!\n", __func__);
			break;
		}

		total_size += data->entry.this_size;

	} while (total_size < data->entry.total_size);

	fclose(fp);

	if (total_size < data->entry.total_size) {
		printf("%s: recv one file failed: %s\n", __func__, rp);
		return -1;
	}

	if (!same_file(rp, md5)) {
		printf("%s: recv one file failed: %s, "
			"md5 not correct!\n", __func__, rp);
		return -1;
	}

recv_one_file_done:
	if (!resp->next)
		*has_more = false;

	set_file_mode(rp, data->entry.mode);

	printf("%s:%d, recv %s done!\n", __func__, __LINE__, rp);


	return 0;

}

int recv_files_from_get_resp(int fd, char *path, struct sftt_packet *resp_packet)
{
	int ret = 0;
	char *rp = NULL;
	struct get_resp *resp = NULL;
	struct get_resp_data *data = NULL;
	int recv_count;


	*has_more = true;

	/* recv file name */
	ret = recv_sftt_packet(client->conn_ctrl.sock, resp_packet);
	if (ret == -1) {
		printf("%s: recv sftt packet failed!\n", __func__);
		return -1;
	}
	resp = resp_packet->obj;
	data = &resp->data;

	if (!(data->total_files > 0)) {
		printf("%s: target file not exist\n", __func__);
		return -1;
	}

	if (data->total_files == 1 && IS_FILE(data->entry.file)) {
		if (is_dir(path))
			rp = path_join(path, data->entry.content);		

		return recv_file_from_get_resp(fd, rp, resp_packet);
	}

	if (!is_dir(path)) {
		printf("%s:%d, when you recv dir or multi files you should specify a dir (path: %s)\n", __func__, __LINE__, path);
		return -1;
	}

	recv_count = 0;
	do {
		rp = path_join(path, data->entry.content);
		recv_file_from_get_resp(fd, rp, resp_packet);	
		recv_count++;
	} while (recv_count < data->total_files);


}

int recv_file_by_put_req(struct client_session *client,
	struct sftt_packet *req_packet, struct sftt_packet *resp_packet,
	struct common_resp *com_resp, bool *has_more)
{
	char *rp = NULL;
	char file[FILE_NAME_MAX_LEN];
	char md5[MD5_STR_LEN];
	FILE *fp = NULL;
	int i = 0, ret = 0, total_size = 0;
	struct put_req *req_info = NULL;
	struct put_req_data *req_data = NULL;
	struct put_resp *resp_info;

	*has_more = true;

	req_info = (struct put_req *)req_packet->obj;
	assert(req_info != NULL);
	req_data = &req_info->data;

	//DEBUG((DEBUG_INFO, "first_idx=%d\n", req_data->file_idx));
	DEBUG((DEBUG_INFO, "file_name=%s\n", (char *)req_data->entry.content));
	DEBUG((DEBUG_INFO, "req_data->total_files=%d|req_data->file_idx=%d\n",
		req_data->total_files, req_data->file_idx));
	//assert(req_info->entry.idx == 0);

	resp_info = (struct put_resp *)mp_malloc(g_mp,
			__func__, sizeof(struct put_resp));
	assert(resp_info != NULL);

	rp = path_join(client->pwd, (char *)req_data->entry.content);

	DEBUG((DEBUG_INFO, "received put req|file=%s\n", rp));
	if (req_data->entry.type == FILE_TYPE_DIR) {
		if (!file_existed(rp)) {
			mkdirp(rp, req_data->entry.mode);
		}

		goto recv_one_file_done;
	}

	/* save file name */
	strncpy(file, (char *)req_data->entry.content, FILE_NAME_MAX_LEN);
	rp = path_join(client->pwd, file);

	/* recv md5 packet */
	DEBUG((DEBUG_INFO, "begin receive file md5 ...\n"));
	ret = recv_sftt_packet(client->connect_fd, req_packet);
	if (!(ret > 0)) {
		printf("recv encountered unrecoverable error ...\n");
		return -1;
	}
	req_info = req_packet->obj;
	req_data = &req_info->data;

	DEBUG((DEBUG_INFO, "file total size: %ld\n", req_data->entry.total_size));

	/* save md5 */
	strncpy(md5, (char *)req_data->entry.content, MD5_STR_LEN);
	if (same_file(rp, md5)) {
		DEBUG((DEBUG_INFO, "file not changed: %s\n", rp));

		/* send resp */
		com_resp->status = RESP_OK;
		resp_packet->obj = com_resp;
		resp_packet->type = PACKET_TYPE_COMMON_RESP;

		ret = send_sftt_packet(client->connect_fd, resp_packet);
		if (ret == -1) {
			printf("%s: send resp failed!\n", __func__);
			return -1;
		}

		goto recv_one_file_done;

	} else {
		/* send resp */
		com_resp->status = RESP_CONTINUE;
		resp_packet->obj = com_resp;
		resp_packet->type = PACKET_TYPE_COMMON_RESP;

		ret = send_sftt_packet(client->connect_fd, resp_packet);
		if (ret == -1) {
			printf("%s: send resp failed!\n", __func__);
			return -1;
		}
	}

	DEBUG((DEBUG_INFO, "begin receive file content ...\n"));

	fp = fopen(rp, "w+");
	if (fp == NULL) {
		printf("create file failed: %s\n", rp);
		return -1;
	}

	i = 0;
	do {
		ret = recv_sftt_packet(client->connect_fd, req_packet);
		if (!(ret > 0)) {
			printf("recv encountered unrecoverable error ...\n");
			break;
		}
		req_info = req_packet->obj;
		req_data = &req_info->data;
		DEBUG((DEBUG_INFO, "receive %d-th block file content|size=%d\n",
			(i + 1), req_data->entry.this_size));

		fwrite(req_data->entry.content, req_data->entry.this_size, 1, fp);

		/* send response */
		resp_info->status = RESP_OK;
		resp_packet->obj = resp_info;
		resp_packet->type = PACKET_TYPE_PUT_RESP;

		ret = send_sftt_packet(client->connect_fd, resp_packet);
		if (ret == -1) {
			printf("send put response failed!\n");
			break;
		}

		total_size += req_data->entry.this_size;
		i += 1;
	} while (total_size < req_data->entry.total_size);

	fclose(fp);

	if (total_size == req_data->entry.total_size) {
		DEBUG((DEBUG_INFO, "received one file: %s\n", rp));
		DEBUG((DEBUG_INFO, "receive file failed: %s\n", rp));
		return -1;
	}

	if (!same_file(rp, md5)) {
		DEBUG((DEBUG_INFO, "recv one file failed: %s|"
			"md5 not correct!\n", rp));
		return -1;
	}

recv_one_file_done:
	if (req_data->file_idx == req_data->total_files - 1)
		*has_more = false;

	set_file_mode(rp, req_data->entry.mode);

	DEBUG((DEBUG_INFO, "recv %s done!\n", rp));

	return 0;
}

int recv_files_by_put_req(int fd, struct sftt_packet *req_packet)
{
	do {
		DEBUG((DEBUG_INFO, "recv %d-th file ...\n", i));
		ret = recv_file_by_put_req(client, req_packet, resp_packet,
			com_resp, &has_more);
		if (ret == -1 || has_more == false)
			break;

		ret = recv_sftt_packet(client->connect_fd, req_packet);
		if (!(ret > 0)) {
			printf("recv encountered unrecoverable error ...\n");
			break;
		}
		++i;
	} while (has_more);

}
