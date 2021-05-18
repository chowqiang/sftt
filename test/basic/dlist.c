#include <stdio.h>
#include "destroy.h"
#include "dlist.h"
#include "show.h"

void show_argv(int argc, char *argv[])
{
	int i;
	for (i = 0; i < argc - 1; ++i) {
		printf("%s ", argv[i]);
	}
	printf("%s\n", argv[i]);
}

int cmp_int(void *a, void *b) {
	return (int)(unsigned long)a - (int)(unsigned long)b;
}

int dlist_test(void) {
	struct dlist list;
	dlist_init(&list, destroy_int);
	dlist_set_show(&list, show_int);
	dlist_show(&list);

	int size = dlist_prepend(&list, (void *)9);
	size = dlist_append(&list, (void *)5);
	size = dlist_ins_next(&list, dlist_tail(&list), (void *)2);
	size = dlist_ins_prev(&list, dlist_tail(&list), (void *)7);
	printf("%d, %d\n", size, dlist_size(&list));
	dlist_show(&list);

	dlist_sort(&list, cmp_int, 1);
	dlist_show(&list);
	printf("%d, %d\n", size, dlist_size(&list));

	size = dlist_pop_front(&list, NULL);
	size = dlist_remove(&list, dlist_head(&list), NULL, 1);
	size = dlist_remove(&list, dlist_head(&list), NULL, 1);
	size = dlist_pop_back(&list, NULL);
	printf("%d, %d\n", size, dlist_size(&list));
	dlist_show(&list);

	dlist_destroy(&list);

	return 0;
}

int main(int argc, char *argv[])
{
	//printf("test file: %s\n", __FILE__);
	//show_argv(argc, argv);
	struct dlist *list = dlist_create(NULL);

	return -1;
}
