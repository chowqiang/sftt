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

void test_mode(void)
{
	char *path = "/root/sftt/README.md";
	char *path2 = "/root/sftt/README.md2";
	FILE *fp = NULL;

	mode_t mode = file_mode(path);
	printf("0x%0x\n", mode);

	fp = fopen(path2, "w");
	fclose(fp);

	set_file_mode(path2, mode);

	mode = file_mode(path);
	printf("0x%0x\n", mode);
}

int main(void)
{
	//test_get_all_file_list();
	test_mode();

	return 0;
}
