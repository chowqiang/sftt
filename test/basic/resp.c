#include <stdio.h>

enum error_code {
	OK,
	ERROR,
	INVALID,
	CNT_CD
};

char *error_message[] = {
	[OK] = "ok",
	[ERROR] = "error",
	[INVALID] = "invalidate",
	[CNT_CD] = "cannot cd"
};

struct resp {
	int code;
	char *msg;
};

int main(void)
{
	struct resp rsp;

	rsp.code = INVALID;
	rsp.msg = error_message[INVALID];

	printf("code: %d, message: %s\n", rsp.code, rsp.msg);

	return 0;	
}
