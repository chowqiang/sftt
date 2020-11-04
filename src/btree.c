#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "btree.h"
#include "destroy.h"

btree_node *btree_node_create(void *data) {
	btree_node *node = (btree_node *)malloc(sizeof(btree_node));
	if (node == NULL) {
		return NULL;
	}

	node->data = data;
	node->left = node->right = NULL;

	return node;
}

void btree_init(btree *tree, void (*destroy) (void *data)) {
	if (tree == NULL) {
		return ;
	}
	tree->size = 0;
	tree->destroy = destroy;
	tree->root = NULL;
}

int btree_node_destroy(btree_node *node, void (*destroy) (void *data)) {
	if (node == NULL) {
		return 0;
	}
	int left_cnt = btree_node_destroy(node->left, destroy);
	int right_cnt = btree_node_destroy(node->right, destroy);
	if (destroy) {
		destroy(node->data);
	}
	free(node);

	return left_cnt + right_cnt + 1;
}

void btree_destroy(btree *tree) {
	if (tree == NULL) {
		return ;
	}
	btree_node_destroy(tree->root, tree->destroy);
	tree->size = 0;
	tree->root = NULL;
}

int btree_ins_left(btree *tree, btree_node *node, void *data) {
	if (tree == NULL) {
		return -1;
	} 
	btree_node *new_node = btree_node_create(data);
	if (new_node == NULL) {
		return -1;
	}

	if (node == NULL && btree_is_empty(tree)) {
		tree->root = new_node;
	} else {
		new_node->left = node->left;
		node->left = new_node;
	}
	tree->size += 1;	

	return tree->size;
}

int btree_is_empty(btree *tree) {
	return tree == NULL ? 1 : tree->root == NULL;
}

int btree_ins_right(btree *tree, btree_node *node, void *data) {
	if (tree == NULL) {
		return -1;
	}

	btree_node *new_node = btree_node_create(data);
	if (new_node == NULL) {
		return -1;
	}
	
	if (node == NULL && btree_is_empty(tree)) {
		tree->root = new_node;
	} else {
		new_node->right = node->right;
		node->right = new_node;
	}
	tree->size += 1;

	return tree->size;
}

int btree_rm_left(btree *tree, btree_node *node) {
	if (tree == NULL || node == NULL) {
		return -1;	
	} 
	int del_cnt = btree_node_destroy(node->left, tree->destroy);
	assert(del_cnt < tree->size);
	tree->size -= del_cnt;
	
	return tree->size;
}

int btree_rm_right(btree *tree, btree_node *node) {
	if (tree == NULL || node == NULL) {
		return -1;
	}
	int del_cnt = btree_node_destroy(node->right, tree->destroy);
	assert(del_cnt < tree->size);
	tree->size -= del_cnt;
	
	return tree->size;
}

int btree_size(btree *tree) {
	return tree == NULL ? 0 : tree->size;
}

btree_node *btree_root(btree *tree) {
	return tree == NULL ? NULL : tree->root;
}

int btree_is_leaf(btree_node *node) {
	return node == NULL ? 0 : node->left == NULL && node->right == NULL;
}

btree_node *btree_node_gen_parent(void *data, btree_node *left, btree_node *right) {
	btree_node *parent = btree_node_create(data);
	if (parent == NULL) {
		return NULL;
	}
	parent->left = left;
	parent->right = right;

	return parent;
}

void *btree_data(btree_node *node) {
	return node == NULL ? NULL : node->data;
}

int btree_show_bfs(btree *tree, void **array) {
	if (tree == NULL || array == NULL) {
		return 0;
	}
	

}

int btree_node_count(btree_node *node) {
	if (node == NULL) {
		return 0;
	} else {
		return btree_node_count(node->left) + btree_node_count(node->right) + 1;
	}
}

#if 0
int main(void) {
	btree tree;
	btree_init(&tree, destroy_int);

	btree_ins_left(&tree, NULL, (void *) 1);
	btree_node *root = btree_root(&tree);
	btree_ins_left(&tree, root, (void *) 2);
	btree_ins_right(&tree, root, (void *) 2);
	printf("%d\n", btree_size(&tree));
	
	btree_destroy(&tree);
	printf("%d\n", btree_size(&tree));
	
	return 0;	
}
#endif
