const XDR_USER_NAME_MAX_LEN = 32;
const XDR_PASSWD_MD5_LEN = 33;
const XDR_SESSION_ID_LEN = 32;
const XDR_DIR_NAME_MAX_LEN = 256;
const XDR_FILE_NAME_MAX_LEN = 256;
const XDR_DIR_PATH_MAX_LEN = 1024;
const XDR_FILE_PATH_MAX_LEN = 1024;
const XDR_FILE_ENTRY_MAX_CNT = 16;
const XDR_CONTENT_BLOCK_SIZE = 2048;
const XDR_NET_MSG_MAX_LEN = 1024;
const XDR_CMD_MAX_LEN = 1024;
const XDR_CMD_RET_BATCH_LEN = 4096;
const XDR_IPV4_MAX_LEN = 16;
const XDR_LOGGED_IN_USER_MAX_CNT = 32;
const XDR_WRITE_MSG_MAX_LEN = 4096;
const XDR_RESP_MESSAGE_MAX_LEN = 80;
const XDR_DIRECT_CMD_RESP_MAX_LEN = 1024;
const XDR_CONNECT_ID_LEN = 32;
const XDR_REQ_RESP_FLAG_NONE = 0x0000;
const XDR_REQ_RESP_FLAG_NEXT = 0x0001;
const XDR_REQ_RESP_FLAG_STOP = 0x0002;

struct version_info {
	short major;
	short minor;
	short revision;
};

struct channel_info_req {
	int flags;
};

struct channel_info_resp_data {
	int main_port;
	int second_port;
};

struct channel_info_resp {
	int status;
	int flags;
	char message[XDR_RESP_MESSAGE_MAX_LEN];
	struct channel_info_resp_data data;
};

struct validate_req {
	int name_len;
	int passwd_len;
	char name[XDR_USER_NAME_MAX_LEN];
	char passwd_md5[XDR_PASSWD_MD5_LEN];
	struct version_info ver;
};

struct validate_resp_data {
	unsigned int uid;
	char name[XDR_USER_NAME_MAX_LEN];
	char session_id[XDR_SESSION_ID_LEN];
	char connect_id[XDR_CONNECT_ID_LEN];
	char pwd[XDR_DIR_PATH_MAX_LEN];
};

struct validate_resp {
	int status;
	int flags;
	char message[XDR_RESP_MESSAGE_MAX_LEN];
	struct validate_resp_data data;
};

struct append_conn_req {
	char session_id[XDR_SESSION_ID_LEN];
	int type;
};

struct append_conn_resp_data {
	char connect_id[XDR_CONNECT_ID_LEN];
};

struct append_conn_resp {
	int status;
	int flags;
	char message[XDR_RESP_MESSAGE_MAX_LEN];
	struct append_conn_resp_data data;
};

struct logged_in_user {
	char session_id[XDR_SESSION_ID_LEN];
	char name[XDR_USER_NAME_MAX_LEN];
	char ip[XDR_IPV4_MAX_LEN];
	int port;
};

struct pwd_req {
	char session_id[XDR_SESSION_ID_LEN];
};

struct pwd_resp_data {
	char pwd[XDR_DIR_PATH_MAX_LEN];
};

struct pwd_resp {
	int status;
	int flags;
	char message[XDR_RESP_MESSAGE_MAX_LEN];
	struct pwd_resp_data data;
};

struct ll_req {
	char session_id[XDR_SESSION_ID_LEN];
	char path[XDR_DIR_PATH_MAX_LEN];
	int to_peer;
	struct logged_in_user user;
};

struct file_entry {
	char name[XDR_FILE_NAME_MAX_LEN];
	unsigned int type;
	unsigned int size;
	unsigned int c_time;
	unsigned int a_time;
	unsigned int m_time;
	unsigned int mode;
};

struct ll_resp_data {
	int total;
	int this_nr;
	struct file_entry entries[XDR_FILE_ENTRY_MAX_CNT];
};

struct ll_resp {
	int status;
	int flags;
	char message[XDR_RESP_MESSAGE_MAX_LEN];
	struct ll_resp_data data;
};

struct cd_req {
	char session_id[XDR_SESSION_ID_LEN];
	char path[XDR_DIR_PATH_MAX_LEN];
};

struct cd_resp_data {
	char pwd[XDR_DIR_PATH_MAX_LEN];
};

struct cd_resp {
	int status;
	int flags;
	char message[XDR_RESP_MESSAGE_MAX_LEN];
	struct cd_resp_data data;
};

struct get_req {
	char session_id[XDR_SESSION_ID_LEN];
	char path[XDR_DIR_PATH_MAX_LEN];
	int to_peer;
	struct logged_in_user user;
};

struct trans_entry {
	int type;
	int this_size;
	unsigned int mode;
	unsigned int total_size;
	unsigned char content[XDR_CONTENT_BLOCK_SIZE];
};

struct get_resp_data {
	int total_files;
	int file_idx;
	struct trans_entry entry;
};

struct get_resp {
	int status;
	int flags;
	char message[XDR_RESP_MESSAGE_MAX_LEN];
	struct get_resp_data data;
	int need_reply;
};

struct put_req_data {
	int total_files;
	int file_idx;
	struct trans_entry entry;
};

struct put_req {
	char session_id[XDR_SESSION_ID_LEN];
	int to_peer;
	struct logged_in_user user;
	struct put_req_data data;
	int flags;
	int need_reply;
};

struct put_resp {
	int status;
	int flags;
	char message[XDR_RESP_MESSAGE_MAX_LEN];
};

struct common_resp {
	int status;
	int flags;
	char message[XDR_RESP_MESSAGE_MAX_LEN];
};

struct write_msg_req {
	int mtype;
	char name[16];
	int pid;
	int length;
	char mtext[XDR_NET_MSG_MAX_LEN];
};

struct read_msg_req {
	int mtype;
};

struct read_msg_resp_data {
	int mtype;
	char name[16];
	int pid;
	int length;
	char mtext[XDR_NET_MSG_MAX_LEN];
};

struct read_msg_resp {
	int status;
	int flags;
	char message[XDR_RESP_MESSAGE_MAX_LEN];
	struct read_msg_resp_data data;
};

struct mp_stat_req {
	char session_id[XDR_SESSION_ID_LEN];
};

struct mp_stat_resp_data {
	unsigned int total_size;
	int total_nodes;
	int using_nodes;
	int free_nodes;
};

struct mp_stat_resp {
	int status;
	int flags;
	char message[XDR_RESP_MESSAGE_MAX_LEN];
	struct mp_stat_resp_data data;
};

struct directcmd_req {
	char session_id[XDR_SESSION_ID_LEN];
	char cmd[XDR_CMD_MAX_LEN];
};

struct directcmd_resp_data {
	int total_len;
	int this_len;
	char content[XDR_DIRECT_CMD_RESP_MAX_LEN];
};

struct directcmd_resp {
	int status;
	int flags;
	char message[XDR_RESP_MESSAGE_MAX_LEN];
	struct directcmd_resp_data data;
};

struct who_req {
	char session_id[XDR_SESSION_ID_LEN];
};

struct who_resp_data {
	int total;
	int this_nr;
	struct logged_in_user users[XDR_LOGGED_IN_USER_MAX_CNT];
};

struct who_resp {
	int status;
	int flags;
	char message[XDR_RESP_MESSAGE_MAX_LEN];
	struct who_resp_data data;
};

struct write_req {
	struct logged_in_user user;
	int len;
	char message[XDR_WRITE_MSG_MAX_LEN];
};

struct write_resp_data {
	char message[XDR_WRITE_MSG_MAX_LEN];
};

struct write_resp {
	int status;
	int flags;
	char message[XDR_RESP_MESSAGE_MAX_LEN];
	struct write_resp_data data;
};

struct port_update_req {
	int second_port;
};

struct port_update_resp {
	int status;
	int flags;
	char message[XDR_RESP_MESSAGE_MAX_LEN];
};

struct reconnect_req {
	char session_id[XDR_SESSION_ID_LEN];
	char connect_id[XDR_CONNECT_ID_LEN];
};

struct reconnect_resp {
	int status;
	int flags;
	char message[XDR_RESP_MESSAGE_MAX_LEN];
};
