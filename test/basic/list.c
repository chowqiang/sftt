#include <stdio.h>
#include <stdlib.h>
#include "list.h"

static LIST_HEAD(purposes);

struct tc_list_node {
	const char *purpose;
	int count;
	struct list_head list;
};

struct tc_list_node *create_list_node(const char *purpose)
{
	struct tc_list_node *node;

	node = malloc(sizeof(struct tc_list_node));
	node->purpose = purpose;
	node->count = 0;

	INIT_LIST_HEAD(&node->list);

	return node;
}

int get_list_node_count(void)
{
	int count = 0;
	struct tc_list_node *node;

	list_for_each_entry(node, &purposes, list)
		++count;

	return count;
}

int main(void)
{
	int count;
	struct tc_list_node *node;

	node = create_list_node("abc");
	//list_add(&purposes, &node->list); // error!
	list_add(&node->list, &purposes);
	printf("list node count: %d\n", get_list_node_count());

	node = create_list_node("abc");
	//list_add(&purposes, &node->list); // error!
	list_add(&node->list, &purposes);
	printf("list node count: %d\n", get_list_node_count());

	node = create_list_node("abc");
	//list_add(&purposes, &node->list); // error!
	list_add(&node->list, &purposes);
	printf("list node count: %d\n", get_list_node_count());

	return 0;
}
