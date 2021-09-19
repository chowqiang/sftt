const USER_NAME_MAX_LEN = 32;
const PASSWD_MD5_LEN = 33;
const SESSION_ID_LEN = 32;
const DIR_PATH_MAX_LEN = 256;
const FILE_NAME_MAX_LEN = 256;
const FILE_ENTRY_MAX_CNT = 16;
const CONTENT_BLOCK_SIZE = 4096;
const NET_MSG_MAX_LEN = 1024;
const CMD_MAX_LEN = 1024;
const CMD_RET_BATCH_LEN = 4096;
const IPV4_MAX_LEN = 16;
const LOGGED_IN_USER_MAX_CNT = 32;
const WRITE_MSG_MAX_LEN = 4096;
const RESP_MESSAGE_MAX_LEN = 1024;

struct version_info {
	short major;
	short minor;
	short revision;
};

struct validate_req {
	struct version_info ver;
	int task_port;
	long name_len;
	long passwd_len;
	char name[USER_NAME_MAX_LEN];
	char passwd_md5[PASSWD_MD5_LEN];
};

struct validate_resp {
	long status;
	long uid;
	char name[USER_NAME_MAX_LEN];
	char session_id[SESSION_ID_LEN];
	char pwd[DIR_PATH_MAX_LEN];
	char message[RESP_MESSAGE_MAX_LEN];
};

struct logged_in_user {
	char session_id[SESSION_ID_LEN];
	char name[USER_NAME_MAX_LEN];
	char ip[IPV4_MAX_LEN];
	int port;
	int task_port;
};

struct pwd_req {
	char session_id[SESSION_ID_LEN];
};

struct pwd_resp {
	long status;
	char pwd[DIR_PATH_MAX_LEN];
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

struct ll_resp {
	int nr;
	struct file_entry entries[FILE_ENTRY_MAX_CNT];
	int idx;
	char message[RESP_MESSAGE_MAX_LEN];
};

struct cd_req {
	char session_id[SESSION_ID_LEN];
	char path[DIR_PATH_MAX_LEN];
};

struct cd_resp {
	long status;
	char pwd[DIR_PATH_MAX_LEN];
};

struct get_req {
	char session_id[SESSION_ID_LEN];
	char path[DIR_PATH_MAX_LEN];
	int to_peer;
	struct logged_in_user user;
};

struct trans_entry {
	long type;
	long total_size;
	long idx;
	long len;
	unsigned long mode;
	unsigned char content[CONTENT_BLOCK_SIZE];
};

struct get_resp {
	long nr;
	long idx;
	struct trans_entry entry;
	char message[RESP_MESSAGE_MAX_LEN];
};

struct put_req {
	char session_id[SESSION_ID_LEN];
	long nr;
	long idx;
	struct trans_entry entry;
	int to_peer;
	struct logged_in_user user;
};

struct put_resp {
	long status;
	char message[RESP_MESSAGE_MAX_LEN];
};

struct common_resp {
	long status;
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

struct read_msg_resp {
	int mtype;
	char name[16];
	int pid;
	int length;
	char mtext[NET_MSG_MAX_LEN];
};

struct mp_stat_req {
	char session_id[SESSION_ID_LEN];
};

struct mp_stat_resp {
	int total_size;
	int total_nodes;
	int using_nodes;
	int free_nodes;
	char message[RESP_MESSAGE_MAX_LEN];
};

struct directcmd_req {
	char session_id[SESSION_ID_LEN];
	char cmd[CMD_MAX_LEN];
};

struct directcmd_resp {
	long total_len;
	long this_len;
	char data[CMD_RET_BATCH_LEN];
};

struct who_req {
	char session_id[SESSION_ID_LEN];
};

struct who_resp {
	long total;
	long num;
	struct logged_in_user users[LOGGED_IN_USER_MAX_CNT];
	char message[RESP_MESSAGE_MAX_LEN];
};

struct write_req {
	struct logged_in_user user;
	int len;
	char message[WRITE_MSG_MAX_LEN];
};

struct write_resp {
	int len;
	char message[WRITE_MSG_MAX_LEN];
};
