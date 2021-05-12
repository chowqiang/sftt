#include <stdio.h>
#include "utils.h"

void test_gen_session_id(void)
{
	char buf[16];

	gen_session_id(buf, 16);
	printf("%s\n", buf);
}

int main(void)
{
	test_gen_session_id();

	return 0;
}
