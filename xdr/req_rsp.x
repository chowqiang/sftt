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

struct validate_req {
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
};

struct put_req {
	char session_id[SESSION_ID_LEN];
	long nr;
	long idx;
	struct trans_entry entry;
};

struct put_resp {
	long status;
};

struct common_resp {
	long status;
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

struct logged_in_user {
	char ip[IPV4_MAX_LEN];
	int port;
	char name[USER_NAME_MAX_LEN];
};

struct who_req {
	char session_id[SESSION_ID_LEN];
};

struct who_resp {
	long total;
	long num;
	struct logged_in_user users[LOGGED_IN_USER_MAX_CNT];
};
