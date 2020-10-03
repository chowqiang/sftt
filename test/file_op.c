#include <unistd.h>
#include <libgen.h> 
#include <sys/stat.h>
#include <string.h>
#include <malloc.h>
#include <dirent.h>

#define FILE_NAME_MAX_LEN	128

typedef struct path_entry {
	char abs_path[FILE_NAME_MAX_LEN];
	char rel_path[FILE_NAME_MAX_LEN];
} path_entry;

typedef struct path_entry_list {
	path_entry entry;
	struct path_entry_list *next;
} path_entry_list;

void printdir(char *dir, int depth)
{
	DIR *dp;
	struct dirent *entry;
	struct stat statbuf;
 
	if ((dp = opendir(dir)) == NULL) {
		fprintf(stderr, "Can`t open directory %s\n", dir);
		return ;
	}
	
	chdir(dir);
	while ((entry = readdir(dp)) != NULL) {
		lstat(entry->d_name, &statbuf);
		if (S_ISDIR(statbuf.st_mode)) {
			if (strcmp(entry->d_name, ".") == 0 || 
				strcmp(entry->d_name, "..") == 0 )  
				continue;	
			printf("%*s%s/\n", depth, "", entry->d_name);
			printdir(entry->d_name, depth+4);
		} else
			printf("%*s%s\n", depth, "", entry->d_name);
	}
	chdir("..");
	closedir(dp);	
}

int is_dir(char *file_name) {
	struct stat file_stat;
	stat(file_name, &file_stat);	

	return S_ISDIR(file_stat.st_mode);
}
int is_file(char *file_name) {
	struct stat file_stat;
	stat(file_name, &file_stat);	

	return S_ISREG(file_stat.st_mode);
}
char *get_base_name(char *file_name) {
	char *base_name = NULL;

	if (is_dir(file_name)) {
		printf("%s is dir\n", file_name);

		char real_path[128];
		int a = realpath(file_name, real_path);
		printf("realpath is: %s, %d\n", real_path, a);

		char *p = basename(real_path);
		int len = strlen(p);
		base_name = (char *)malloc(sizeof(char) * (len + 1));
		strcpy(base_name, p);
			
	} else if (is_file(file_name)) {
		printf("%s is file\n", file_name);

		char *p = basename(file_name);
		int len = strlen(p);
		base_name = (char *)malloc(sizeof(char) * (len + 1));
		strcpy(base_name, p);

	} else {
		printf("cannot recognize %s file type\n", file_name);
	}

	return base_name;
}
int main2(int argc, char **argv) {		
	if (argc < 2) {
		printf("Usage: %s file|dir\n", argv[0]);
		return -1;
	}
	char *base_name = get_base_name(argv[1]);
	if (base_name) {
		printf("base name is %s\n", base_name);
		free(base_name);
	}
	
	return 0;
}
path_entry *get_file_path_entry(char *file_name) {
	path_entry *pe = (path_entry *)malloc(sizeof(path_entry));
	if (pe == NULL) {
		return NULL;
	}	
	realpath(file_name, pe->abs_path);
	char *p = basename(pe->abs_path);
	strcpy(pe->rel_path, p);
	
	return pe;
}
void free_path_entry_list(path_entry_list *head) {
	path_entry_list *p = head, *q = head;
	while (p) {
		q = p->next;
		free(p);
		p = q;
	}
}
path_entry_list *get_dir_path_entry_list(char *file_name, char *prefix) {
	/*
	path_entry_list *head = (path_entry_list *)malloc(sizeof(path_entry_list));
	if (head == NULL) {
		return NULL;
	}
	*/
	path_entry_list *head = NULL;
	path_entry_list *current_entry = NULL;
	path_entry_list *sub_list = NULL;

	char dir_abs_path[FILE_NAME_MAX_LEN];
	char dir_rel_path[FILE_NAME_MAX_LEN];
	char tmp_path[FILE_NAME_MAX_LEN];

	realpath(file_name, dir_abs_path);
	char *p = basename(dir_abs_path);
	sprintf(dir_rel_path, "%s/%s", prefix, p);
		
	DIR *dp;
	struct dirent *entry;
	struct stat statbuf;
 
	if ((dp = opendir(file_name)) == NULL) {
		printf("cannot open dir: %s\n", file_name);
		return NULL;
	}
	
	chdir(file_name);
	while ((entry = readdir(dp)) != NULL) {
		sprintf(tmp_path, "%s/%s", dir_abs_path, entry->d_name);
		lstat(tmp_path, &statbuf);
		if (S_ISDIR(statbuf.st_mode)) {
			if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
				continue;
			}
			//printf("dir tmp path: %s\n", tmp_path);
			sub_list = get_dir_path_entry_list(tmp_path, dir_rel_path);	
			if (sub_list == NULL) {
				continue;	
			}

			if (current_entry == NULL) {
				current_entry = head = sub_list;
			} else {
				current_entry->next = sub_list;
			}

			while (current_entry->next) {
				current_entry = current_entry->next;
			}				
				
		} else {
			path_entry_list *node = (path_entry_list *)malloc(sizeof(path_entry_list));
			if (node == NULL) {
				continue;
			}

			sprintf(node->entry.abs_path, "%s/%s", dir_abs_path, entry->d_name);
			sprintf(node->entry.rel_path, "%s/%s", dir_rel_path, entry->d_name);
			//printf("file: %s\n", node->entry.abs_path);
			node->next = NULL;

			if (current_entry == NULL) {
				current_entry = head = node;
			} else {
				current_entry->next = node;
				current_entry = node;
			}
		}
	}
	closedir(dp);	

	return head;
}
int main(int argc, char **argv) {
	if (argc < 2) {
		printf("Usage: %s file|dir\n", argv[0]);
		return -1;
	}
	if (is_file(argv[1])) {
		path_entry *pe = get_file_path_entry(argv[1]);	
		if (pe != NULL) {
			printf("abs_path: %s, rel_path: %s\n", pe->abs_path, pe->rel_path);
			free(pe);
		}
	} else if (is_dir(argv[1])) {
		char prefix[1] = {0};
		path_entry_list *pes = get_dir_path_entry_list(argv[1], prefix);
		if (pes != NULL) {
			path_entry_list *p = pes;
			while (p) {
				printf("abs_path: %s, rel_path: %s\n", p->entry.abs_path, p->entry.rel_path);
				p = p->next;
			}
			free_path_entry_list(pes);
		}
	}

	return 0;
}
