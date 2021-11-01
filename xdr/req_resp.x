const USER_NAME_MAX_LEN = 32;
const PASSWD_MD5_LEN = 33;
const SESSION_ID_LEN = 32;
const DIR_PATH_MAX_LEN = 256;
const FILE_NAME_MAX_LEN = 256;
const FILE_ENTRY_MAX_CNT = 16;
const CONTENT_BLOCK_SIZE = 2048;
const NET_MSG_MAX_LEN = 1024;
const CMD_MAX_LEN = 1024;
const CMD_RET_BATCH_LEN = 4096;
const IPV4_MAX_LEN = 16;
const LOGGED_IN_USER_MAX_CNT = 32;
const WRITE_MSG_MAX_LEN = 4096;
const RESP_MESSAGE_MAX_LEN = 128;
const DIRECT_CMD_RESP_MAX_LEN = 1024;
const CONNECT_ID_LEN = 32;

struct version_info {
	short major;
	short minor;
	short revision;
};

struct validate_req {
	int name_len;
	int passwd_len;
	char name[USER_NAME_MAX_LEN];
	char passwd_md5[PASSWD_MD5_LEN];
	struct version_info ver;
};

struct validate_resp_data {
	long uid;
	char name[USER_NAME_MAX_LEN];
	char session_id[SESSION_ID_LEN];
	char connect_id[CONNECT_ID_LEN];
	char pwd[DIR_PATH_MAX_LEN];
};

struct validate_resp {
	int status;
	int next;
	char message[RESP_MESSAGE_MAX_LEN];
	struct validate_resp_data data;
};

struct append_conn_req {
	char session_id[SESSION_ID_LEN];
	int type;
};

struct append_conn_resp_data {
	char connect_id[CONNECT_ID_LEN];
};

struct append_conn_resp {
	int status;
	int next;
	char message[RESP_MESSAGE_MAX_LEN];
	struct append_conn_resp_data data;
};

struct logged_in_user {
	char session_id[SESSION_ID_LEN];
	char name[USER_NAME_MAX_LEN];
	char ip[IPV4_MAX_LEN];
	int port;
};

struct pwd_req {
	char session_id[SESSION_ID_LEN];
};

struct pwd_resp_data {
	char pwd[DIR_PATH_MAX_LEN];
};

struct pwd_resp {
	int status;
	int next;
	char message[RESP_MESSAGE_MAX_LEN];
	struct pwd_resp_data data;
};

struct ll_req {
	char session_id[SESSION_ID_LEN];
	char path[DIR_PATH_MAX_LEN];
	int to_peer;
	struct logged_in_user user;
};

struct file_entry {
	char name[FILE_NAME_MAX_LEN];
	unsigned long mode;
	long type;
	long size;
	long c_time;
	long a_time;
	long m_time;
};

struct ll_resp_data {
	int total;
	int this_nr;
	struct file_entry entries[FILE_ENTRY_MAX_CNT];
};

struct ll_resp {
	int status;
	int next;
	char message[RESP_MESSAGE_MAX_LEN];
	struct ll_resp_data data;
};

struct cd_req {
	char session_id[SESSION_ID_LEN];
	char path[DIR_PATH_MAX_LEN];
};

struct cd_resp_data {
	char pwd[DIR_PATH_MAX_LEN];
};

struct cd_resp {
	int status;
	int next;
	char message[RESP_MESSAGE_MAX_LEN];
	struct cd_resp_data data;
};

struct get_req {
	char session_id[SESSION_ID_LEN];
	char path[DIR_PATH_MAX_LEN];
	int to_peer;
	struct logged_in_user user;
};

struct trans_entry {
	int type;
	long total_size;
	int this_size;
	unsigned long mode;
	unsigned char content[CONTENT_BLOCK_SIZE];
};

struct get_resp_data {
	int total_files;
	int file_idx;
	struct trans_entry entry;
};

struct get_resp {
	int status;
	int next;
	char message[RESP_MESSAGE_MAX_LEN];
	struct get_resp_data data;
	int need_reply;
};

struct put_req_data {
	int total_files;
	int file_idx;
	struct trans_entry entry;
};

struct put_req {
	char session_id[SESSION_ID_LEN];
	int to_peer;
	struct logged_in_user user;
	struct put_req_data data;
	int next;
	int need_reply;
};

struct put_resp {
	int status;
	int next;
	char message[RESP_MESSAGE_MAX_LEN];
};

struct common_resp {
	int status;
	int next;
	char message[RESP_MESSAGE_MAX_LEN];
};

struct write_msg_req {
	int mtype;
	char name[16];
	int pid;
	int length;
	char mtext[NET_MSG_MAX_LEN];
};

struct read_msg_req {
	int mtype;
};

struct read_msg_resp_data {
	int mtype;
	char name[16];
	int pid;
	int length;
	char mtext[NET_MSG_MAX_LEN];
};

struct read_msg_resp {
	int status;
	int next;
	char message[RESP_MESSAGE_MAX_LEN];
	struct read_msg_resp_data data;
};

struct mp_stat_req {
	char session_id[SESSION_ID_LEN];
};

struct mp_stat_resp_data {
	long total_size;
	int total_nodes;
	int using_nodes;
	int free_nodes;
};

struct mp_stat_resp {
	int status;
	int next;
	char message[RESP_MESSAGE_MAX_LEN];
	struct mp_stat_resp_data data;
};

struct directcmd_req {
	char session_id[SESSION_ID_LEN];
	char cmd[CMD_MAX_LEN];
};

struct directcmd_resp_data {
	int total_len;
	int this_len;
	char content[DIRECT_CMD_RESP_MAX_LEN];
};

struct directcmd_resp {
	int status;
	int next;
	char message[RESP_MESSAGE_MAX_LEN];
	struct directcmd_resp_data data;
};

struct who_req {
	char session_id[SESSION_ID_LEN];
};

struct who_resp_data {
	int total;
	int this_nr;
	struct logged_in_user users[LOGGED_IN_USER_MAX_CNT];
};

struct who_resp {
	int status;
	int next;
	char message[RESP_MESSAGE_MAX_LEN];
	struct who_resp_data data;
};

struct write_req {
	struct logged_in_user user;
	int len;
	char message[WRITE_MSG_MAX_LEN];
};

struct write_resp_data {
	char message[WRITE_MSG_MAX_LEN];
};

struct write_resp {
	int status;
	int next;
	char message[RESP_MESSAGE_MAX_LEN];
	struct write_resp_data data;
};
