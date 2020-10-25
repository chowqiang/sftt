#include "compress.h"

#define CHARSET_SIZE	256

typedef struct {

} char_stat;

typedef struct btree {
	int freq;
	unsigned char c;
	struct btree *left;
	struct btree *right;
} btree;

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
btree *generate_huffman_tree(int *char_freq) {
	char visited[CHARSET_SIZE];
	memset(visited, 0, sizeof(visited));
	int min_index = 0;
	unsigned char min_freq_char = 0;
	for (;;) {
		min_index = get_min_freq_char(char_freq, visited);
		if (min_index == -1) {
			break;
		}			
	}
		

}

int huffman_compress(unsigned char *input, int input_len, unsigned char **output, int *output_len) {
	if (input == NULL || input_len < 1) {
		return -1;
	}
	if (output == NULL || *output == NULL || output_len == NULL) {
		return -1;
	}
	
	int char_freq[CHARSET_SIZE];
	int ret = calc_char_freq(input, input_len, char_freq);
	if (ret == -1) {
		return -1;
	}
	btree *root = generate_huffman_tree(char_freq);	
		
}

int huffman_decompress(unsigned char *input, int input_len, unsigned char **output, int *output_len) {
	if (input == NULL || input_len < 1) {
		return -1;
	}
	if (output == NULL || *output == NULL || output_len == NULL) {
		return -1;
	}

}



