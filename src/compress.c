#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include "compress.h"
#include "dlist.h"
#include "btree.h"
#include "stack.h"
#include "map.h"

#define CHARSET_SIZE	256

typedef struct {
	int ch;
	int freq;
} char_stat_node;

char_stat_node *create_char_stat_node(int ch, int freq) {
	char_stat_node *node = (char_stat_node *)malloc(sizeof(char_stat_node));
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
	btree_node *t_node = (btree_node *)data;
	char_stat_node *cs_node = (char_stat_node *)t_node->data;
	if (cs_node == NULL) {
		return ;
	}
	if (cs_node->ch == -1) {
		printf("%d(%d) ", cs_node->freq, cs_node->ch);	
	} else {
		printf("%d(%c) ", cs_node->freq, cs_node->ch);
	}
}

void free_char_stata_node(char_stat_node *node) {
	if (node) {
		free(node);
	}
}

int calc_char_freq(unsigned char *input, int input_len, int *char_freq) {
	if (input == NULL || input_len < 1) {
		printf("params error! func: %a, line: %d\n", __func__, __LINE__);
		return -1;
	}
	memset(char_freq, 0, sizeof(int) * CHARSET_SIZE);
	int i = 0;
	for (i = 0; i < input_len; ++i) {
		char_freq[input[i]]++;
	}
	
	return 0;
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

btree_node *fetch_min_btree_node(dlist *list) {
	if (list == NULL || dlist_is_empty(list)) {
		return NULL;
	}

	dlist_node *ln = list->head;
	dlist_node *min_ln = ln;

	btree_node *min_tn = (btree_node *)min_ln->data;
	btree_node *tn = NULL;

	char_stat_node *csn1 = NULL, *csn2 = NULL;

	ln = ln->next;
	dlist_for_each_pos(ln) {
		tn = (btree_node *)ln->data;
		csn1 = (char_stat_node *)tn->data;
		csn2 = (char_stat_node *)min_tn->data;
		if (csn1->freq < csn2->freq) {
			min_tn = tn;
			min_ln = ln;
		}
	}	
	dlist_remove(list, min_ln, NULL, 1);
 
	return min_tn;		
}

btree *generate_huffman_tree(int *char_freq) {
	if (char_freq == NULL) {
		return NULL;
	}
	dlist list;
	dlist_init(&list, NULL);
	btree_node *t_node = NULL, *t_node1 = NULL, *t_node2 = NULL;
	char_stat_node *cs_node = NULL;
	
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
		dlist_append(&list, (void *)t_node);	
	}
#if DEBUG
	dlist_set_show(&list, show_char_stat_by_btree_node);
	dlist_show(&list);
#endif
	for (;;) {
		t_node1 = fetch_min_btree_node(&list);
		assert(t_node1 != NULL);
		t_node2 = fetch_min_btree_node(&list);
		if (t_node2 == NULL) {
			break;
		}
		sum = ((char_stat_node *)t_node1->data)->freq + ((char_stat_node *)t_node2->data)->freq;	
		cs_node = create_char_stat_node(-1, sum); 
		t_node = btree_node_gen_parent((void *)cs_node, t_node1, t_node2);
		assert(t_node != NULL);
		dlist_append(&list, (void *)t_node);
	}

	btree *tree = btree_create(NULL);
	btree_set_root(tree, t_node1);

	return tree;	
}

char *get_char_code(stack *s) {
	int len = stack_size(s);
	void **array = (void **)malloc(sizeof(void *) * len); 
	char *code = (char *)malloc(sizeof(char) * (len + 1));
	if (array == NULL || code == NULL) {
		return NULL;
	}

	stack_peek_all(s, array);
	int i = 0;
	for (i = 0; i < len; ++i) {
		code[i] = (char)array[i];
	}	
	code[len] = 0;	

	free(array);

	return code;	
}

int get_char_codes(btree *tree, char *char_codes[CHARSET_SIZE]) {
	int i = 0;
	void *data = NULL;
	if (tree == NULL || tree->root == NULL || char_codes == NULL) {
		return -1;
	}
	map *m = map_create();
	btree_node *root = tree->root;
	btree_node *tn = NULL;
	char_stat_node *csn = NULL;

	stack *sc = stack_create(NULL);
	stack *stn = stack_create(NULL);
	stack_push(stn, root->right);


	while (!stack_is_empty(stn)) {
		stack_pop(stn, (void **)&tn);
		if (tn == NULL) {
			continue;
		}
		if (tn->left) {
			if (map_find(m, tn->left, NULL) == -1) {
				stack_push(sc, (void *)'1');
				stack_push(stn, tn->left);
				map_add(m, (void *)tn->left, NULL);
			} else {
				stack_pop(sc, NULL);
			}
		} else if (tn->right) { 
			if (map_find(m, tn->right, NULL) == -1) {
				stack_push(sc, (void *)'0');
				stack_push(stn, tn->right);
				map_add(m, (void *)tn->right, NULL);
			} else {
				stack_pop(sc, NULL);
			}
		} else {
			csn = (char_stat_node *)tn->data;
			char_codes[csn->ch] = get_char_code(sc);
			printf("%c: %s\n", csn->ch, char_codes[csn->ch]);
			stack_pop(sc, NULL);
		}
		
	}

	return 0;
}

void free_char_codes(char *char_codes[CHARSET_SIZE]) {
	int i = 0;
	for (i = 0; i < CHARSET_SIZE; ++i) {
		if (char_codes[i]) {
			free(char_codes[i]);
			char_codes[i] = NULL;
		}
	}
}

int huffman_compress(unsigned char *input, int input_len, unsigned char **output) {
	if (input == NULL || input_len < 1) {
		printf("params error!\n");
		return -1;
	}
	if (output == NULL || *output == NULL) {
		printf("params error!\n");
		return -1;
	}
	
	int char_freq[CHARSET_SIZE];
	int ret = calc_char_freq(input, input_len, char_freq);
	if (ret == -1) {
		printf("calc char freq failed!\n");
		return -1;
	}
	btree *tree = generate_huffman_tree(char_freq);	
	if (tree == NULL) {
		return -1;
	}
	char *char_codes[CHARSET_SIZE];
	memset(char_codes, 0, sizeof(char_codes));
	get_char_codes(tree, char_codes);
#if DEBUG
	dlist *bfs_list = btree_bfs(tree);
	dlist_set_show(bfs_list, show_char_stat_by_btree_node);
	dlist_show(bfs_list);
#endif
	free_char_codes(char_codes);

	return 0;	
}

int huffman_decompress(unsigned char *input, int input_len, unsigned char **output) {
	if (input == NULL || input_len < 1) {
		return -1;
	}
	if (output == NULL || *output == NULL) {
		return -1;
	}

	return 0;
}

#if 1
int main(void) {
	unsigned char *text = "aaaabbbccd";
	int text_len = strlen(text);
	unsigned char *output = (unsigned char *)malloc(sizeof(unsigned char) * 16);
	
	huffman_compress(text, text_len, &output);

	return 0;
}
#endif 

