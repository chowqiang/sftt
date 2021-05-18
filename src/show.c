#include <stdio.h>
#include "show.h"

void show_int(void *data) {
	printf("%d ", (int)(unsigned long)data);
}
