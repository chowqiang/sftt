const USER_NAME_MAX_LEN = 32;
const PASSWD_MD5_LEN = 33;
const SESSION_ID_LEN = 32;
const DIR_PATH_MAX_LEN = 256;

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
};

struct pwd_req {
	char session_id[SESSION_ID_LEN];
};

struct pwd_resp {
	int status;
	char pwd[DIR_PATH_MAX_LEN];
};
