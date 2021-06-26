#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "dlist.h"
#include "file.h"

void test_path_ops(void)
{
	char *path = "../../var/sftt/root";
	char *rp = realpath(path, NULL);
	char pwd[128];
	
	getcwd(pwd, 127);

	printf("%s\n", pwd);
	
	if (rp == NULL) {
		printf("get realpath failed!\n");
	} else {
		printf("%s\n", realpath(rp, NULL));
		printf("%s\n", basename(rp));	
	}
}

void test_get_all_file_list(void)
{
	struct dlist *list;
	struct dlist_node *node;

	list = get_all_file_list("../sftt");
#if 1
	dlist_for_each(list, node)
		printf("%s\n", node->data);
#endif
}

int main(void)
{
	test_get_all_file_list();

	return 0;
}
