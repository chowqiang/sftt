#include <stdio.h>
#include <stdbool.h>

bool is_ok() {
	return true;
}
int main(void) {
	printf(PROC_NAME "\n");
	bool ret = is_ok();
	if (ret) {
		printf("ok\n");
	} else {
		printf("not ok\n");
	}
	return 0;
}
