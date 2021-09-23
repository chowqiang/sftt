#include <stdio.h>
#include <stdlib.h>
#include "list.h"

static LIST_HEAD(purposes);

struct tc_list_node {
	const char *purpose;
	int count;
	struct list_head list;
};

struct test_client {
	int nr;
	struct list_head friends;
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

void test_list_add(void)
{
	//int count;
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
}

void test_list_foreach(void)
{
	struct tc_list_node *nodes;
	struct tc_list_node *p, *q;

	nodes = malloc(sizeof(struct tc_list_node));
	nodes->purpose = "abc";

	INIT_LIST_HEAD(&nodes->list);

	list_for_each_entry(p, &nodes->list, list)
		printf("%s\n", p->purpose);

	q = malloc(sizeof(struct tc_list_node));
	q->purpose = "def";

	list_add(&q->list, &nodes->list);

	list_for_each_entry(p, &nodes->list, list)
		printf("%s\n", p->purpose);
}

void test_list_del(void)
{
	struct test_client client;
	struct tc_list_node *p, *q;

	INIT_LIST_HEAD(&client.friends);

	p = malloc(sizeof(struct tc_list_node));
	p->purpose = "abc";
	list_add(&p->list, &client.friends);

	p = malloc(sizeof(struct tc_list_node));
	p->purpose = "def";
	list_add(&p->list, &client.friends);

	list_for_each_entry(p, &client.friends, list)
		printf("%s\n", p->purpose);

	list_for_each_entry_safe(p, q, &client.friends, list) {
		list_del(&p->list);
		free(p);
	}

	list_for_each_entry(p, &client.friends, list)
		printf("%s\n", p->purpose);

	if (list_empty(&client.friends)) {
		printf("list is empty!\n");
	} else {
		printf("list is not empty!\n");
	}

}

int main(void)
{
	//test_list_add();
	//test_list_foreach();
	test_list_del();

	return 0;
}
