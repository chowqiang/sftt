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

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include "bits.h"
#include "btree.h"
#include "compress.h"
#include "dlist.h"
#include "map.h"
#include "mem_pool.h"
#include "stack.h"
#include "utils.h"

struct char_stat_node {
	int ch;
	int freq;
};

extern struct mem_pool *g_mp;

struct char_stat_node *create_char_stat_node(int ch, int freq)
{

	struct char_stat_node *node = (struct char_stat_node *)mp_malloc(
			g_mp, __func__, sizeof(struct char_stat_node));
	if (node == NULL) {
		return NULL;
	}
	node->ch = ch;
	node->freq = freq;

	return node;
}

void show_char_stat_by_btree_node(void *data)
{
	if (data == NULL) {
		return ;	
	}
	struct btree_node *t_node = (struct btree_node *)data;
	struct char_stat_node *cs_node = (struct char_stat_node *)t_node->data;
	if (cs_node == NULL) {
		return ;
	}

	if (cs_node->ch == -1) {
		printf("%d(%d) ", cs_node->freq, cs_node->ch);	
	} else {
		printf("%d(%c) ", cs_node->freq, cs_node->ch);
	}
}

void free_char_stata_node(struct char_stat_node *node)
{
	if (node) {
		mp_free(g_mp, node);
	}
}

int calc_char_freq(unsigned char *input, int input_len, int *char_freq)
{
	if (input == NULL || input_len < 1) {
		printf("params error! func: %s, line: %d\n", __func__, __LINE__);
		return -1;
	}
	memset(char_freq, 0, sizeof(int) * CHARSET_SIZE);

	int i = 0;
	for (i = 0; i < input_len; ++i) {
		char_freq[input[i]]++;
	}
	
	return 0;
}

void show_char_freq(int *char_freq)
{
	int i = 0, j = 0;
	int sum = 0;
	for (i = 0; i < CHARSET_SIZE; ++i) {
		if (char_freq[i]) {
			printf("%c: %d\n", i, char_freq[i]);
			++j;
			sum += char_freq[i];
		}
	}
	printf("char freq summary: char count: %d, freq count: %d\n", j, sum);
}

int get_min_freq_char(int *char_freq, char *visited)
{
	int min = 0, min_index = -1, i = 0;
	for (i = 0; i < CHARSET_SIZE; ++i) {
		if (char_freq[i] == 0 || visited[i]) {
			continue;
		}
		if (min_index == -1 || min < char_freq[i]) {
			min = char_freq[i];
			min_index = i;
		}
	}

	return min_index;
}

struct btree_node *fetch_min_btree_node(struct dlist *list)
{
	if (list == NULL || dlist_empty(list)) {
		return NULL;
	}

	struct dlist_node *ln = list->head;
	struct dlist_node *min_ln = ln;

	struct btree_node *min_tn = (struct btree_node *)min_ln->data;
	struct btree_node *tn = NULL;

	struct char_stat_node *csn1 = NULL, *csn2 = NULL;

	ln = ln->next;
	dlist_for_each_pos(ln) {
		tn = (struct btree_node *)ln->data;
		csn1 = (struct char_stat_node *)tn->data;
		csn2 = (struct char_stat_node *)min_tn->data;
		if (csn1->freq < csn2->freq) {
			min_tn = tn;
			min_ln = ln;
		}
	}	
	dlist_remove(list, min_ln, NULL, 1);

	return min_tn;		
}

struct btree *generate_huffman_tree(int *char_freq)
{
	if (char_freq == NULL) {
		return NULL;
	}

	struct dlist *list = dlist_create(FREE_MODE_NOTHING);
	assert(list != NULL);

	struct btree_node *t_node = NULL, *t_node1 = NULL, *t_node2 = NULL;
	struct char_stat_node *cs_node = NULL;
	
	int i = 0;
	int sum = 0;
	for (i = 0; i < CHARSET_SIZE; ++i) {
		if (char_freq[i] == 0) {
			continue;
		}

		cs_node = create_char_stat_node(i, char_freq[i]);
		assert(cs_node != NULL);

		t_node = btree_node_create(cs_node);
		assert(t_node != NULL);

		dlist_append(list, (void *)t_node);	
	}

	int ch1 = 0, ch2 = 0;
	int freq1 = 0, freq2 = 0;
	for (;;) {
		t_node1 = fetch_min_btree_node(list);
		assert(t_node1 != NULL);
#ifdef CONFIG_HUFFMAN_TREE_DEBUG
		ch1 = ((struct char_stat_node *)t_node1->data)->ch;
		freq1 = ((struct char_stat_node *)t_node1->data)->freq;
		if (ch1 == -1) {
			printf("%d(%d),", ch1, freq1);
		} else {
			printf("%c(%d),", ch1, freq1);
		}
#endif
		t_node2 = fetch_min_btree_node(list);
		if (t_node2 == NULL) {
			if (btree_is_leaf(t_node1)) {
				sum = ((struct char_stat_node *)t_node1->data)->freq;
				cs_node = create_char_stat_node(-1, sum);
				assert(cs_node != NULL);

				t_node = btree_node_gen_parent((void *)cs_node,
						t_node1, t_node2);
				assert(t_node != NULL);
				t_node1 = t_node;
			}
			break;
		}
#ifdef CONFIG_HUFFMAN_TREE_DEBUG
		ch2 = ((struct char_stat_node *)t_node2->data)->ch;
		freq2 = ((struct char_stat_node *)t_node2->data)->freq;
		if (ch2 == -1) {
			printf("%d(%d)\n", ch2, freq2);
		} else {
			printf("%c(%d)\n", ch2, freq2);
		}
#endif
		sum = ((struct char_stat_node *)t_node1->data)->freq +
			((struct char_stat_node *)t_node2->data)->freq;
		cs_node = create_char_stat_node(-1, sum);
		assert(cs_node != NULL);

		t_node = btree_node_gen_parent((void *)cs_node, t_node1, t_node2);
		assert(t_node != NULL);

		dlist_append(list, (void *)t_node);
	}
#ifdef CONFIG_HUFFMAN_TREE_DEBUG
	printf("\n");
#endif

	struct btree *tree = btree_create(FREE_MODE_MP_FREE);
	btree_set_root(tree, t_node1);

	dlist_destroy(list);

	return tree;
}

char *get_char_code(struct stack *s)
{
	int i = 0;
	int len = stack_size(s);

	void **array = (void *)mp_malloc(g_mp, "get_char_code_array",
			sizeof(void *) * len);
	char *code = (char *)mp_malloc(g_mp, "get_char_code_code",
			sizeof(char) * (len + 1));
	if (array == NULL || code == NULL) {
		return NULL;
	}

	stack_peek_all(s, array);
	for (i = 0; i < len; ++i) {
		code[len - 1 - i] = (char)(unsigned long)array[i];
	}	
	code[len] = 0;	

	mp_free(g_mp, array);

	return code;	
}

void show_char_stat_node(char *prefix, struct char_stat_node *csn)
{
	if (csn->ch == -1) {
		printf("%s: %d, %d\n", prefix, csn->ch, csn->freq);
	} else {
		printf("%s: %c, %d\n", prefix, csn->ch, csn->freq);
	}
}

int get_char_codes(struct btree *tree, char *char_codes[CHARSET_SIZE])
{
	struct map *m = map_create();
	struct btree_node *root = NULL;
	struct btree_node *tn = NULL;
	struct char_stat_node *csn = NULL;

	struct stack *sc = stack_create(FREE_MODE_NOTHING);
	struct stack *stn = stack_create(FREE_MODE_NOTHING);

	int ret = 0;

	if (tree == NULL || tree->root == NULL || char_codes == NULL) {
		ret = -1;
		goto get_char_codes_done;
	}

	root = tree->root;
	stack_push(stn, root);

	/*
	 * Access each node by DFS and record the codes of leaf.
	 */
	while (!stack_is_empty(stn)) {
		tn = stack_peek(stn);
		if (tn == NULL) {
			continue;
		}

		if (btree_is_leaf(tn)) {
			csn = (struct char_stat_node *)tn->data;
			char_codes[csn->ch] = get_char_code(sc);
			//printf("%c: %s\n", csn->ch, char_codes[csn->ch]);
			stack_pop(sc, NULL);
			stack_pop(stn, NULL);
			continue;
		}

		if (tn->left && map_find(m, ptr_equal, tn->left, NULL) == -1) {
			stack_push(sc, (void *)'1');
			stack_push(stn, tn->left);
			map_add(m, (void *)tn->left, NULL);

		} else if (tn->right && map_find(m, ptr_equal, tn->right, NULL) == -1) {
			stack_push(sc, (void *)'0');
			stack_push(stn, tn->right);
			map_add(m, (void *)tn->right, NULL);

		} else {
			stack_pop(sc, NULL);
			stack_pop(stn, NULL);
		}
	}

get_char_codes_done:
	map_destroy(m);
	stack_destroy(sc);
	stack_destroy(stn);

	return ret;
}

void free_char_codes(char *char_codes[CHARSET_SIZE])
{
	int i = 0;
	for (i = 0; i < CHARSET_SIZE; ++i) {
		if (char_codes[i]) {
			mp_free(g_mp, char_codes[i]);
			char_codes[i] = NULL;
		}
	}
}

int copy_char_freq(unsigned char *pos, int *char_freq)
{
	int i = 0;
	int cnt = 0;
	unsigned char *head = pos++;

	for (i = 0; i < CHARSET_SIZE; ++i) {
		if (char_freq[i]) {
			*(pos++) = (unsigned char)i;
			memcpy(pos, &char_freq[i], sizeof(int));
			pos += sizeof(int);
			++cnt;
		}
	}
	*head = (unsigned char)cnt;

	return (pos - head);
}

/*
 * Generate codes for each byte and put it into encoded bytes.
 */
int huffman_encode(unsigned char *input, int input_len,
		char *char_codes[CHARSET_SIZE], unsigned char *pos)
{

	int i = 0, j = 0, index = CHAR_BIT_LEN;
	unsigned char *head = pos;
	char *tmp;

#ifdef CONFIG_HUFFMAN_COMPRESS_DEBUG
	printf("huffman_encode input_len: %d\n", input_len);
#endif
	memcpy(head, &input_len, sizeof(int));
	pos += sizeof(int);

	tmp = NULL;
	for (i = 0; i < input_len; ++i) {
		tmp = char_codes[input[i]];
		if (tmp == NULL) {
			printf("0x%0x\n", input[i]);
			assert(tmp != NULL);
		}
		for (j = 0; tmp[j]; ++j) {
			if (index == 0) {	
				++pos;
				index = CHAR_BIT_LEN;
			}
			set_bit(pos, index--, tmp[j] == '0' ? 0 : 1);
		}	
	}

	return (pos - head + 1);
}

void show_char_codes(struct btree *tree, char *char_codes[CHARSET_SIZE])
{
	int i = 0;
	struct dlist *bfs_list = btree_bfs(tree);

	dlist_set_show(bfs_list, show_char_stat_by_btree_node);
	dlist_show(bfs_list);

	for (i = 0; i < CHARSET_SIZE; ++i) {
		if (char_codes[i] == NULL) {
			continue;
		}
		printf("%c: %s\n", i, char_codes[i]);
	}

	dlist_destroy(bfs_list);
}

int get_compress_output_len(unsigned char *input, int input_len,
	int char_freq[CHARSET_SIZE], char *char_codes[CHARSET_SIZE])
{
	int i, tmp_len, output_len;
	char *tmp_ptr;

	/* char number cost 1 byte */
	output_len = 1;

	for (i = 0; i < CHARSET_SIZE; ++i) {
		/* pair of <char, freq> cost 5 byte */
		if (char_freq[i])
			output_len += (1 + sizeof(int));
	}

	/* text len cost 4 byte */
	output_len += sizeof(int);

	tmp_len = 0;
	for (i = 0; i < input_len; ++i) {
		tmp_ptr = char_codes[input[i]];
		tmp_len += strlen(tmp_ptr);
	}

	output_len += (ALIGN_CEIL(tmp_len, 8) >> 3);

	return output_len;
}

/*
 * The process of huffman compress contains:
 * 1) Calculate the frequency of chars.
 * 2) Generate huffman tree by the frequency of chars.
 * 3) Generate the codes of chars by huffman tree.
 * 4) Replace the chars by their codes.
 */
int huffman_compress(unsigned char *input, int input_len,
	unsigned char **output)
{
	int char_freq[CHARSET_SIZE];
	struct btree *tree;
	char *char_codes[CHARSET_SIZE];
	unsigned char *pos;
	int ret, char_freq_len, char_code_len;
	int output_len;

	if (input == NULL || input_len < 1) {
		printf("%s:%d, params error!\n", __func__, __LINE__);
		return -1;
	}

	if (output == NULL) {
		printf("%s:%d, params error!\n", __func__, __LINE__);
		return -1;
	}

	/*
	 * Calculate the frequency of chars.
	 */
	ret = calc_char_freq(input, input_len, char_freq);
	if (ret == -1) {
		printf("calc char freq failed!\n");
		return -1;
	}

#ifdef CONFIG_HUFFMAN_COMPRESS_DEBUG
	show_char_freq(char_freq);
#endif

	/*
	 * Generate huffman tree by the frequency of chars.
	 * The node of huffman tree is the frequency of char.
	 */
	tree = generate_huffman_tree(char_freq);
	if (tree == NULL) {
		return -1;
	}

	memset(char_codes, 0, sizeof(char_codes));
	/*
	 * Get the codes of chars according to the huffman tree.
	 */
	if (get_char_codes(tree, char_codes) == -1) {
		printf("get char codes failed!\n");
		return -1;
	}
	btree_destroy(tree);

	output_len = get_compress_output_len(input, input_len, char_freq, char_codes);
	*output = mp_malloc(g_mp, "output_len", output_len * sizeof(unsigned char));
	pos = *output;
	/*
	 * Put the frequency of chars into encoded bytes.
	 */
	char_freq_len = copy_char_freq(pos, char_freq);

#ifdef CONFIG_HUFFMAN_COMPRESS_DEBUG
	printf("char freq block size: %d\n", char_freq_len);
#endif

	char_code_len = huffman_encode(input, input_len, char_codes,
			pos + char_freq_len);

	free_char_codes(char_codes);

	assert(char_freq_len + char_code_len == output_len);

	return output_len;
}

unsigned char *get_char_freq(int char_freq[CHARSET_SIZE], unsigned char *pos)
{
	int i = 0;
	unsigned char ch = 0;
	int char_cnt = 0;

	if (char_freq == NULL || pos == NULL) {
		printf("get_char_freq params error!\n");
		return NULL;
	}
	memset(char_freq, 0, CHARSET_SIZE * sizeof(int));

	char_cnt = (int)(*pos++);

	for (i = 0; i < char_cnt; ++i) {
		ch = *pos++;
		memcpy(&char_freq[ch], pos, sizeof(int));
		pos += sizeof(int);	
	}
	
	return pos;
}

int huffman_decode(struct btree *tree, unsigned char *input, int input_len,
		unsigned char *output)
{
	struct btree_node *root = NULL, *tn = NULL;
	struct char_stat_node *csn = NULL;
	int i = 0, j = 0, index = CHAR_BIT_LEN;
	unsigned char *pos = input, bit = 0;

	if (tree == NULL || tree->root == NULL || input == NULL ||
			input_len < 1 || output == NULL) {
		printf("huffman decode pararms error!\n");
		return 0;
	}

	root = tree->root;
	for (i = 0; i < input_len; ++i) {
		tn = root;
		/*
		 * The leaf of huffman tree stores the info of char.
		 */
		while (!btree_is_leaf(tn)) {
			if (index == 0) {
				index = CHAR_BIT_LEN;
				++pos;
			}
			bit = get_bit(*pos, index--);	
			if (bit == 0) {
				tn = tn->right;
			} else {
				tn = tn->left;
			}
		}
		csn = (struct char_stat_node *)tn->data;
		output[j++] = (char)csn->ch;
//		printf("%c", csn->ch);
	}

	return j;
}

int huffman_decompress(unsigned char *input, unsigned char **output)
{
	int char_freq[CHARSET_SIZE];
	unsigned char *pos = input;
	struct btree *tree;
	int input_len, output_len;

	if (input == NULL || output == NULL) {
		printf("decompress: input and output cannot be NULL!\n");
		return -1;
	}

	/*
	 * Get the frequency of chars from encoded bytes.
	 */
	pos = get_char_freq(char_freq, pos);
#ifdef CONFIG_HUFFMAN_DECOMPRESS_DEBUG
	show_char_freq(char_freq);
#endif

	/*
	 * Generate the huffman tree by the frequency of chars.
	 */
	tree = generate_huffman_tree(char_freq);
	if (tree == NULL) {
		printf("decompress: generate_huffman_tree failed!\n");
		return -1;
	}

	input_len = *(int *)pos;
#ifdef CONFIG_HUFFMAN_DECOMPRESS_DEBUG
	printf("encoded char count: %d\n", input_len);
#endif
	pos += sizeof(int);

	output_len = input_len;
	*output = mp_malloc(g_mp, "output_len", output_len * sizeof(unsigned char));

	huffman_decode(tree, pos, input_len, *output);

	btree_destroy(tree);
	
	return output_len;
}
