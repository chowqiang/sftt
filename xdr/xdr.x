const USER_NAME_MAX_LEN = 33;
const PASSWD_MAX_LEN = 17;
const SESSION_ID_LEN = 17;

struct validate_req {
	int name_len;
	int passwd_len;
	char name[USER_NAME_MAX_LEN];
	char passwd_md5[PASSWD_MAX_LEN];
};

struct validate_resp {
	int status;
	uint64_t uid;
	char name[USER_NAME_MAX_LEN];
	char session_id[SESSION_ID_LEN];
};
