#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "compress.h"
#include "dlist.h"
#include "btree.h"

#define CHARSET_SIZE	256

typedef struct {
	int val;
} char_stat;

int calc_char_freq(unsigned char *input, int input_len, int *char_freq) {
	if (input == NULL || input_len < 1) {
		return -1;
	}
	memset(char_freq, 0, sizeof(int) * CHARSET_SIZE);
	int i = 0;
	for (i = 0; i < input_len; ++i) {
		char_freq[input[i]]++;
	}
	
	return -1;
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
	dlist_node *p = list->head;
	dlist_node *l_node = NULL;
	btree_node *min_t_node = (btree_node *)p->data;
	btree_node *t_node = NULL;
	p = p->next;	
	while (p) {
		t_node = (btree_node *)p->data;
		if ((int)t_node->data < (int)min_t_node->data) {
			min_t_node = t_node;
			l_node = p;
		}
		p = p->next;
	}
	dlist_remove(list, l_node, NULL);
 
	return min_t_node;		
}
btree *generate_huffman_tree(int *char_freq) {
	if (char_freq == NULL) {
		return NULL;
	}
	dlist list;
	dlist_init(&list, NULL);
	btree_node *t_node = NULL, *t_node1 = NULL, *t_node2 = NULL;
	
	int i = 0;
	int sum = 0;
	for (i = 0; i < CHARSET_SIZE; ++i) {
		if (char_freq[i] == 0) {
			continue;
		}
		t_node = btree_node_create((void *)char_freq[i]);
		assert(t_node != NULL);
		dlist_append(&list, (void *)t_node);	
	}
	for (;;) {
		t_node1 = fetch_min_btree_node(&list);
		assert(t_node1 != NULL);
		t_node2 = fetch_min_btree_node(&list);
		if (t_node2 == NULL) {
			break;
		}
		sum = (int)t_node1->data + (int)t_node2->data;	
		t_node = btree_node_gen_parent((void *)sum, t_node1, t_node2);
		assert(t_node != NULL);
		dlist_append(&list, (void *)t_node);
	}
	btree_node *root = t_node1;	
}

int huffman_compress(unsigned char *input, int input_len, unsigned char **output) {
	if (input == NULL || input_len < 1) {
		return -1;
	}
	if (output == NULL || *output == NULL) {
		return -1;
	}
	
	int char_freq[CHARSET_SIZE];
	int ret = calc_char_freq(input, input_len, char_freq);
	if (ret == -1) {
		return -1;
	}
	btree *tree = generate_huffman_tree(char_freq);	
	
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



