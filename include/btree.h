#ifndef _BTREE_H_
#define _BTREE_H_

#include "dlist.h"

struct btree_node {
	void *data;
	struct btree_node *left;
	struct btree_node *right;
};

struct btree {
	int size;
	void (*destroy) (void *data);
	struct btree_node *root;
};

/**
 *
 **/
struct btree_node *btree_node_create(void *data); 

/**
 *
 **/
struct btree *btree_create(void (*destroy)(void *data)); 

/**
 *
 **/
void btree_init(struct btree *tree, void (*destroy) (void *data));

/**
 *
 **/
void btree_destroy(struct btree *tree);

/**
 *
 **/
int btree_ins_left(struct btree *tree, struct btree_node *node, void *data);

/**
 *
 **/
int btree_ins_right(struct btree *tree, struct btree_node *node, void *data);

/**
 *
 **/
int btree_rm_left(struct btree *tree, struct btree_node *node);

/**
 *
 **/
int btree_rm_right(struct btree *tree, struct btree_node *node);

/**
 *
 **/
int btree_size(struct btree *tree);

/**
 *
 **/
struct btree_node *btree_root(struct btree *tree);

/**
 *
 **/
int btree_is_leaf(struct btree_node *node);

/**
 *
 **/
void *btree_data(struct btree_node *node);

/**
 *
 **/
int btree_is_empty(struct btree *tree);

/**
 *
 **/
struct btree_node *btree_node_gen_parent(void *data, struct btree_node *left, struct btree_node *right);

/**
 *
 **/
struct dlist *btree_bfs(struct btree *tree); 

/**
 *
 **/
int btree_node_count(struct btree_node *node);

/**
 *
 **/
void btree_set_root(struct btree *tree, struct btree_node *root);

#endif
