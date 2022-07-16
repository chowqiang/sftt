#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define THREAD_NUM 4

pthread_key_t key;

struct thread_context {
	int idx;
};

void *task(void *arg)
{
	int *p = NULL;
	struct thread_context *ctx = arg;

	sleep(THREAD_NUM - ctx->idx);
	printf("pthread %d begin to exec ...\n", ctx->idx);
	if (ctx->idx == 0) {
		*p = 1;
	}

	for (;;);

	return NULL;
}

void general_routine(void)
{
	struct thread_context *ctx;

	ctx = (struct thread_context *)pthread_getspecific(key);

	printf("id: %d\n", ctx->idx);
}

void *task2(void *arg)
{
	pthread_setspecific(key, arg);

	general_routine();

	return NULL;
}

int pthread_dummy(int ret)
{
	return ret;
}

void test_pthread_fault(void)
{
	int i;
	void *val;
	pthread_t pids[THREAD_NUM];
	struct thread_context ctxs[THREAD_NUM];

	pthread_key_create(&key, NULL);
	for (i = 0; i < THREAD_NUM; ++i) {
		ctxs[i].idx = i;
		pthread_create(&pids[i], NULL, task2, &ctxs[i]);
	}

	for (i = 0; i < THREAD_NUM; ++i) {
		pthread_join(pids[i], &val);
		printf("thread %d has done!\n", i);
	}

	printf("all thread done!\n");
}

int main(void)
{
	test_pthread_fault();

	return 0;
}
