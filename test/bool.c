#include <stdio.h>
bool is_ok() {
	return true;
}
int main(void) {
	bool ret = is_ok();
	if (ret) {
		printf("ok\n");
	} else {
		printf("not ok\n");
	}
	return 0;
}
