/*
 * Copyright (C)  2020-2021 Min Zhou <zhoumin@bupt.cn>, all rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "btree.h"
#include "debug.h"
#include "mem_pool.h"
#include "queue.h"
#include "destroy.h"

extern struct mem_pool *g_mp;

struct btree_node *btree_node_create(void *data)
{
	DBUG_ENTER(__func__);

	struct btree_node *node = (struct btree_node *)mp_malloc(g_mp,
		__func__, sizeof(struct btree_node));
	if (node == NULL) {
		DBUG_RETURN(NULL);
	}

	node->data = data;
	node->left = node->right = NULL;

	DBUG_RETURN(node);
}

struct btree *btree_create(enum free_mode mode)
{
	DBUG_ENTER(__func__);

	struct btree *tree = (struct btree *)mp_malloc(g_mp,
		__func__, sizeof(struct btree));
	if (tree == NULL) {
		DBUG_RETURN(NULL);
	}
	btree_init(tree, mode);

	DBUG_RETURN(tree);
}

void btree_init(struct btree *tree, enum free_mode mode)
{
	DBUG_ENTER(__func__);

	if (tree == NULL) {
		DBUG_VOID_RETURN;
	}
	tree->size = 0;
	tree->free_mode = mode;
	tree->root = NULL;

	DBUG_VOID_RETURN;
}

void btree_set_root(struct btree *tree, struct btree_node *root)
{
	DBUG_ENTER(__func__);

	if (tree == NULL) {
		DBUG_VOID_RETURN;
	}
	tree->root = root;
	tree->size = btree_node_count(root);

	DBUG_VOID_RETURN;
}

int btree_node_destroy(struct btree_node *node,
	enum free_mode mode)
{
	DBUG_ENTER(__func__);

	if (node == NULL) {
		DBUG_RETURN(0);
	}
	int left_cnt = btree_node_destroy(node->left, mode);
	int right_cnt = btree_node_destroy(node->right, mode);

	switch (mode)
	{
	case FREE_MODE_NOTHING:
		break;
	case FREE_MODE_MP_FREE:
		mp_free(g_mp, node->data);
		break;
	case FREE_MODE_FREE:
		free(node->data);
		break;
	}
	mp_free(g_mp, node);

	DBUG_RETURN(left_cnt + right_cnt + 1);
}

void btree_destroy(struct btree *tree)
{
	DBUG_ENTER(__func__);

	if (tree == NULL) {
		DBUG_VOID_RETURN;
	}
	btree_node_destroy(tree->root, tree->free_mode);
	tree->size = 0;
	tree->root = NULL;
	mp_free(g_mp, tree);

	DBUG_VOID_RETURN;
}

int btree_ins_left(struct btree *tree, struct btree_node *node, void *data)
{
	DBUG_ENTER(__func__);

	if (tree == NULL) {
		DBUG_RETURN(-1);
	}
	struct btree_node *new_node = btree_node_create(data);
	if (new_node == NULL) {
		DBUG_RETURN(-1);
	}

	if (node == NULL && btree_is_empty(tree)) {
		tree->root = new_node;
	} else {
		new_node->left = node->left;
		node->left = new_node;
	}
	tree->size += 1;

	DBUG_RETURN(tree->size);
}

int btree_is_empty(struct btree *tree)
{
	DBUG_ENTER(__func__);

	DBUG_RETURN(tree == NULL ? 1 : tree->root == NULL);
}

int btree_ins_right(struct btree *tree, struct btree_node *node,
	void *data)
{
	DBUG_ENTER(__func__);

	if (tree == NULL) {
		DBUG_RETURN(-1);
	}

	struct btree_node *new_node = btree_node_create(data);
	if (new_node == NULL) {
		DBUG_RETURN(-1);
	}

	if (node == NULL && btree_is_empty(tree)) {
		tree->root = new_node;
	} else {
		new_node->right = node->right;
		node->right = new_node;
	}
	tree->size += 1;

	DBUG_RETURN(tree->size);
}

int btree_rm_left(struct btree *tree, struct btree_node *node)
{
	DBUG_ENTER(__func__);

	if (tree == NULL || node == NULL) {
		DBUG_RETURN(-1);
	}

	int del_cnt = btree_node_destroy(node->left, tree->free_mode);
	assert(del_cnt < tree->size);
	tree->size -= del_cnt;

	DBUG_RETURN(tree->size);
}

int btree_rm_right(struct btree *tree, struct btree_node *node)
{
	DBUG_ENTER(__func__);

	if (tree == NULL || node == NULL) {
		DBUG_RETURN(-1);
	}
	int del_cnt = btree_node_destroy(node->right, tree->free_mode);
	assert(del_cnt < tree->size);
	tree->size -= del_cnt;

	DBUG_RETURN(tree->size);
}

int btree_size(struct btree *tree)
{
	DBUG_ENTER(__func__);

	DBUG_RETURN(tree == NULL ? 0 : tree->size);
}

struct btree_node *btree_root(struct btree *tree)
{
	DBUG_ENTER(__func__);

	DBUG_RETURN(tree == NULL ? NULL : tree->root);
}

int btree_is_leaf(struct btree_node *node)
{
	DBUG_ENTER(__func__);

	DBUG_RETURN(node == NULL ? 0 : node->left == NULL && node->right == NULL);
}

struct btree_node *btree_node_gen_parent(void *data, struct btree_node *left,
	struct btree_node *right)
{
	DBUG_ENTER(__func__);

	struct btree_node *parent = btree_node_create(data);
	if (parent == NULL) {
		DBUG_RETURN(NULL);
	}
	parent->left = left;
	parent->right = right;

	DBUG_RETURN(parent);
}

void *btree_data(struct btree_node *node)
{
	DBUG_ENTER(__func__);

	DBUG_RETURN(node == NULL ? NULL : node->data);
}

struct dlist *btree_bfs(struct btree *tree)
{
	DBUG_ENTER(__func__);

	if (tree == NULL) {
		DBUG_RETURN(NULL);
	}

	struct dlist *list = dlist_create(FREE_MODE_NOTHING);
	if (list == NULL) {
		DBUG_RETURN(NULL);
	}

	struct btree_node *node = NULL;
	struct queue *q = queue_create(FREE_MODE_NOTHING);

	queue_enqueue(q, tree->root);
	while (!queue_is_empty(q)) {
		queue_dequeue(q, (void **)&node);
		dlist_append(list, node);
		if (node->left) {
			queue_enqueue(q, node->left);
		}
		if (node->right) {
			queue_enqueue(q, node->right);
		}
	}

	queue_destroy(q);

	DBUG_RETURN(list);
}

int btree_node_count(struct btree_node *node)
{
	DBUG_ENTER(__func__);

	if (node == NULL) {
		DBUG_RETURN(0);
	} else {
		DBUG_RETURN(btree_node_count(node->left) + btree_node_count(node->right) + 1);
	}
}

int btree_test(void)
{
	DBUG_ENTER(__func__);

	struct btree tree;
	btree_init(&tree, FREE_MODE_NOTHING);

	btree_ins_left(&tree, NULL, (void *) 1);
	struct btree_node *root = btree_root(&tree);
	btree_ins_left(&tree, root, (void *) 2);
	btree_ins_right(&tree, root, (void *) 2);
	printf("%d\n", btree_size(&tree));

	btree_destroy(&tree);
	printf("%d\n", btree_size(&tree));

	DBUG_RETURN(0);
}
