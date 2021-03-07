#include <stdio.h>

#define ASIZE	10
#define BSIZE	20

#define TYPE	"FILE:"

void test(void)
{
	char header[ASIZE + BSIZE];
	printf("%d\n", sizeof(header));
	
	printf("%d\n", sizeof(TYPE));
}

void test1(void)
{
	printf("sizeof(size_t): %d, sizeof(int): %d\n", sizeof(size_t), sizeof(int));
}

int main(void) {
	test1();
	
	return -1;
}
