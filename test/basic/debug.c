#include <pthread.h>
#include <stdio.h>
#include "debug.h"

#define THREAD_NUM	2

void *thread_func(void *arg)
{
	DBUG_ENTER(__func__);
	printf("pthread-%d\n", (int)(unsigned long)(arg));
	DBUG_RETURN(NULL);
}

void test_debug(void)
{
	pthread_t ids[THREAD_NUM];
	int i = 0;
	int ret = 0;
	void *val = NULL;

	for (i = 0; i < THREAD_NUM; ++i) {
		ret = pthread_create(&ids[i], NULL, thread_func, (void *)(unsigned long)i);
		if (ret) {
			printf("%s:%d, pthread create failed!\n", __func__, __LINE__);
		}
	}

	for (i = 0; i < THREAD_NUM; ++i)
		pthread_join(ids[i], &val);
}

int main(void)
{
	test_debug();

	return 0;
}
