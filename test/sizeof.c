#include <stdio.h>

#define ASIZE	10
#define BSIZE	20

#define TYPE	"FILE:"

int main(void) {
	char header[ASIZE + BSIZE];
	printf("%d\n", sizeof(header));
	
	printf("%d\n", sizeof(TYPE));
	
	return -1;
}
