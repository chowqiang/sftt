#ifndef _BTREE_H_
#define _BTREE_H_
typedef struct _btree_node {
	void *data;
	struct _btree_node *left;
	struct _btree_node *right;
} btree_node;

typedef struct _btree {
	int size;
	void (*destroy) (void *data);
	btree_node *root;
} btree;

void btree_init(btree *tree, void (*destroy) (void *data));

void btree_destroy(btree *tree);

int btree_ins_left(btree *tree, btree_node *node, void *data);

int btree_ins_right(btree *tree, btree_node *node, void *data);

int btree_rm_left(btree *tree, btree_node *node);

int btree_rm_right(btree *tree, btree_node *node);

int btree_size(btree *tree);

btree_node *btree_root(btree *tree);

int btree_is_leaf(btree_node *node);

void btree_data(btree_node *node);

#endif
