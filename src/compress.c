#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include "bits.h"
#include "btree.h"
#include "compress.h"
#include "dlist.h"
#include "file.h"
#include "map.h"
#include "mem_pool.h"
#include "stack.h"

struct char_stat_node {
	int ch;
	int freq;
};

extern struct mem_pool *g_mp;

struct char_stat_node *create_char_stat_node(int ch, int freq) {

	struct char_stat_node *node = (struct char_stat_node *)mp_malloc(g_mp, sizeof(struct char_stat_node));
	//assert(node != NULL);
	if (node == NULL) {
		return NULL;
	}
	node->ch = ch;
	node->freq = freq;

	return node;
}

void show_char_stat_by_btree_node(void *data) {
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

void free_char_stata_node(struct char_stat_node *node) {
	if (node) {
		mp_free(g_mp, node);
	}
}

int calc_char_freq(unsigned char *input, int input_len, int *char_freq) {
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

void show_char_freq(int *char_freq) {
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

int get_min_freq_char(int *char_freq, char *visited) {
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

struct btree_node *fetch_min_btree_node(struct dlist *list) {
	if (list == NULL || dlist_is_empty(list)) {
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

struct btree *generate_huffman_tree(int *char_freq) {
	if (char_freq == NULL) {
		return NULL;
	}

	struct dlist *list = dlist_create(NULL);
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
#if 0
		ch1 = ((char_stat_node *)t_node1->data)->ch;
		freq1 = ((char_stat_node *)t_node1->data)->freq;	
		if (ch1 == -1) {
			printf("%d(%d),", ch1, freq1);
		} else {
			printf("%c(%d),", ch1, freq1);  
		}
#endif
		t_node2 = fetch_min_btree_node(list);
		if (t_node2 == NULL) {
			break;
		}
#if	0
		ch2 = ((char_stat_node *)t_node2->data)->ch;
		freq2 = ((char_stat_node *)t_node2->data)->freq;	
		if (ch2 == -1) {
			printf("%d(%d)\n", ch2, freq2);
		} else {
			printf("%c(%d)\n", ch2, freq2);  
		}
#endif
		sum = ((struct char_stat_node *)t_node1->data)->freq + ((struct char_stat_node *)t_node2->data)->freq;	
		cs_node = create_char_stat_node(-1, sum); 
		assert(cs_node != NULL);

		t_node = btree_node_gen_parent((void *)cs_node, t_node1, t_node2);
		assert(t_node != NULL);

		dlist_append(list, (void *)t_node);
	}
	//printf("\n");

	struct btree *tree = btree_create(NULL);
	btree_set_root(tree, t_node1);

	return tree;	
}

char *get_char_code(struct stack *s) {
	int len = stack_size(s);

	void **array = (void **)mp_malloc(g_mp, sizeof(void *) * len);
	char *code = (char *)mp_malloc(g_mp, sizeof(char) * (len + 1));
	if (array == NULL || code == NULL) {
		return NULL;
	}

	stack_peek_all(s, array);
	int i = 0;
	for (i = 0; i < len; ++i) {
		code[len - 1 - i] = (char)array[i];
	}	
	code[len] = 0;	

	mp_free(g_mp, array);

	return code;	
}

void show_char_stat_node(char *prefix, struct char_stat_node *csn) {
	if (csn->ch == -1) {
		printf("%s: %d, %d\n", prefix, csn->ch, csn->freq);
	} else {
		printf("%s: %c, %d\n", prefix, csn->ch, csn->freq);
	}
}

int get_char_codes(struct btree *tree, char *char_codes[CHARSET_SIZE]) {
	if (tree == NULL || tree->root == NULL || char_codes == NULL) {
		return -1;
	}

	struct map *m = map_create();
	struct btree_node *root = tree->root;
	struct btree_node *tn = NULL;
	struct char_stat_node *csn = NULL;

	struct stack *sc = stack_create(NULL);
	struct stack *stn = stack_create(NULL);
	stack_push(stn, root);

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

		if (tn->left && map_find(m, tn->left, NULL) == -1) {
			stack_push(sc, (void *)'1');
			stack_push(stn, tn->left);
			map_add(m, (void *)tn->left, NULL);

		} else if (tn->right && map_find(m, tn->right, NULL) == -1) { 
			stack_push(sc, (void *)'0');
			stack_push(stn, tn->right);
			map_add(m, (void *)tn->right, NULL);

		} else {
			stack_pop(sc, NULL);
			stack_pop(stn, NULL);
		}
	}

	return 0;
}

void free_char_codes(char *char_codes[CHARSET_SIZE]) {
	int i = 0;
	for (i = 0; i < CHARSET_SIZE; ++i) {
		if (char_codes[i]) {
			mp_free(g_mp, char_codes[i]);
			char_codes[i] = NULL;
		}
	}
}

int copy_char_freq(unsigned char *pos, int *char_freq) {
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

int huffman_encode(unsigned char *input, int input_len, char *char_codes[CHARSET_SIZE], unsigned char *pos) {
	//printf("huffman_encode input_len: %d\n", input_len);

	int i = 0, j = 0, index = CHAR_BIT_LEN;
	unsigned char *head = pos;
	memcpy(head, &input_len, sizeof(int));
	pos += sizeof(int);

	char *tmp = NULL;
	for (i = 0; i < input_len; ++i) {
		tmp = char_codes[input[i]];
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

void show_char_codes(struct btree *tree, char *char_codes[CHARSET_SIZE]) {
	struct dlist *bfs_list = btree_bfs(tree);
	dlist_set_show(bfs_list, show_char_stat_by_btree_node);
	dlist_show(bfs_list);
	int i = 0;
	for (i = 0; i < CHARSET_SIZE; ++i) {
		if (char_codes[i] == NULL) {
			continue;
		}
		printf("%c: %s\n", i, char_codes[i]);
	}
}

int huffman_compress(unsigned char *input, int input_len, unsigned char *output) {
	if (input == NULL || input_len < 1) {
		printf("params error!\n");
		return -1;
	}
	if (output == NULL) {
		printf("params error!\n");
		return -1;
	}
	
	int char_freq[CHARSET_SIZE];
	int ret = calc_char_freq(input, input_len, char_freq);
	if (ret == -1) {
		printf("calc char freq failed!\n");
		return -1;
	}
	//show_char_freq(char_freq);

	struct btree *tree = generate_huffman_tree(char_freq);	
	if (tree == NULL) {
		return -1;
	}

	char *char_codes[CHARSET_SIZE];
	memset(char_codes, 0, sizeof(char_codes));
	get_char_codes(tree, char_codes);

	unsigned char *pos = output;
	int char_freq_len = copy_char_freq(pos, char_freq);	
	//printf("char freq block size: %d\n", char_freq_len);

	int char_code_len = huffman_encode(input, input_len, char_codes, pos + char_freq_len);	

	free_char_codes(char_codes);

	return char_freq_len + char_code_len;
}

unsigned char *get_char_freq(int char_freq[CHARSET_SIZE], unsigned char *pos) {
	if (char_freq == NULL || pos == NULL) {
		printf("get_char_freq params error!\n");
		return NULL;
	}
	memset(char_freq, 0, CHARSET_SIZE * sizeof(int));

	int i = 0;
	unsigned char ch = 0;
	int char_cnt = (int)(*pos++);
	for (i = 0; i < char_cnt; ++i) {
		ch = *pos++;
		memcpy(&char_freq[ch], pos, sizeof(int));
		pos += sizeof(int);	
	}
	
	return pos;
}

int huffman_decode(struct btree *tree, unsigned char *input, int input_len, unsigned char *output) {
	if (tree == NULL || tree->root == NULL || input == NULL || input_len < 1 || output == NULL) {
		printf("huffman decode pararms error!\n");
		return 0;
	}	
	struct btree_node *root = tree->root, *tn = NULL;
	struct char_stat_node *csn = NULL;
	int i = 0, j = 0, index = CHAR_BIT_LEN;
	unsigned char *pos = input, bit = 0;

	for (i = 0; i < input_len; ++i) {
		tn = root;
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

int huffman_decompress(unsigned char *input, unsigned char *output) {
	if (input == NULL || output == NULL) {
		printf("decompress: input and output cannot be NULL!\n");
		return -1;
	}

	int char_freq[CHARSET_SIZE];
	unsigned char *pos = input;
	pos = get_char_freq(char_freq, pos);
	//show_char_freq(char_freq);

	struct btree *tree = generate_huffman_tree(char_freq);	
	if (tree == NULL) {
		printf("decompress: generate_huffman_tree failed!\n");
		return -1;
	}

	int input_len = *(int *)pos;
	//printf("encoded char count: %d\n", input_len);
	pos += sizeof(int);

	int out_len = huffman_decode(tree, pos, input_len, output);
	
	return out_len;
}

void test_basic(void) {
	int i = 0;
	unsigned char *text = "aaaabbbccd";
	int text_len = strlen(text);

	unsigned char *output = (unsigned char *)mp_malloc(g_mp, sizeof(unsigned char) * 1024);
	int out_len = huffman_compress(text, text_len, output);
	for (i = 0; i < out_len; ++i) {
		printf("%0x", output[i]);
	}
	printf("\n");
}

void test_file(void) {
	size_t file_size = 0;

	unsigned char *contents = file_get_contents("./dlist.c", &file_size);
	if (contents == NULL) {
		printf("get file contents failed!\n");
		return ;
	}

	unsigned char *encodes = (unsigned char *)mp_malloc(g_mp, CHARSET_SIZE * sizeof(int) + 5 * file_size);
	if (encodes == NULL) {
		printf("alloc merroy for encode failed!\n");
		return ;
	}

	int encode_len = huffman_compress(contents, file_size, encodes);
	printf("compress: file_size: %d, encode_len: %d\n", file_size, encode_len);
	
	unsigned char *decodes = (unsigned char *)mp_malloc(g_mp, file_size + 1);
	if (decodes == NULL) {
		printf("alloc merroy for decode failed!\n");
		return ;
	}	
	
	int decode_len = huffman_decompress(encodes, decodes);
	printf("decompress: encode_len: %d, decode_len: %d\n", encode_len, decode_len);

	file_put_contents("./tmp.txt", decodes, (size_t)decode_len);

	return ;	
}
#if 0
int main(void) {
	test_file();
}
#endif 

