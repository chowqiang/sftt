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
	enum free_mode free_mode;
	struct btree_node *root;
};

/*
 *
 */
struct btree_node *btree_node_create(void *data); 

/*
 *
 */
struct btree *btree_create(enum free_mode mode);

/*
 *
 */
void btree_init(struct btree *tree, enum free_mode mode);

/*
 *
 */
void btree_destroy(struct btree *tree);

/*
 *
 */
int btree_ins_left(struct btree *tree, struct btree_node *node, void *data);

/*
 *
 */
int btree_ins_right(struct btree *tree, struct btree_node *node, void *data);

/*
 *
 */
int btree_rm_left(struct btree *tree, struct btree_node *node);

/*
 *
 */
int btree_rm_right(struct btree *tree, struct btree_node *node);

/*
 *
 */
int btree_size(struct btree *tree);

/*
 *
 */
struct btree_node *btree_root(struct btree *tree);

/*
 *
 */
int btree_is_leaf(struct btree_node *node);

/*
 *
 */
void *btree_data(struct btree_node *node);

/*
 *
 */
int btree_is_empty(struct btree *tree);

/*
 *
 */
struct btree_node *btree_node_gen_parent(void *data, struct btree_node *left, struct btree_node *right);

/*
 *
 */
struct dlist *btree_bfs(struct btree *tree); 

/*
 *
 */
int btree_node_count(struct btree_node *node);

/*
 *
 */
void btree_set_root(struct btree *tree, struct btree_node *root);

#endif
