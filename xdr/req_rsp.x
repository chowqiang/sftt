const USER_NAME_MAX_LEN = 32;
const PASSWD_MD5_LEN = 33;
const SESSION_ID_LEN = 32;
const DIR_PATH_MAX_LEN = 256;
const FILE_NAME_MAX_LEN = 256;
const FILE_ENTRY_MAX_CNT = 16;
const CONTENT_BLOCK_SIZE = 4096;

struct validate_req {
	int name_len;
	int passwd_len;
	char name[USER_NAME_MAX_LEN];
	char passwd_md5[PASSWD_MD5_LEN];
};

struct validate_resp {
	int status;
	int uid;
	char name[USER_NAME_MAX_LEN];
	char session_id[SESSION_ID_LEN];
	char pwd[DIR_PATH_MAX_LEN];
};

struct pwd_req {
	char session_id[SESSION_ID_LEN];
};

struct pwd_resp {
	int status;
	char pwd[DIR_PATH_MAX_LEN];
};

struct ll_req {
	char session_id[SESSION_ID_LEN];
	char path[DIR_PATH_MAX_LEN];
};

struct file_entry {
	char name[FILE_NAME_MAX_LEN];
	int type;
	int size;
	int c_time;
	int a_time;
	int m_time;
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
	int status;
	char pwd[DIR_PATH_MAX_LEN];
};

struct get_req {
	char session_id[SESSION_ID_LEN];
	char path[DIR_PATH_MAX_LEN];
};

struct trans_entry {
	int type;
	int total_size;
	int idx;
	int len;
	unsigned char content[CONTENT_BLOCK_SIZE];
};

struct get_resp {
	int nr;
	int idx;
	struct trans_entry entry;
};

struct put_req {
	char session_id[SESSION_ID_LEN];
	int nr;
	int idx;
	struct trans_entry entry;
};

struct put_resp {
	int status;
};
