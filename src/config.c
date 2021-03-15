#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "config.h"
#include "sftt_strings.h"

void strip(char *line) {
	int len = strlen(line);
	
	while (len > 0 && line[len - 1] == ' ') {
		line[--len] = 0;
	}				
	
	if (len < 1) {
		return ;
	}
	
	int i = 0;
	while (i < len && line[i] == ' ') {
		++i;
	}
	if (i >= len) {
		line[0] = 0;
		return ;
	}
		
	int j = 0;
	for (j = 0; i <= len; ++i, ++j) {
		line[j] = line[i];
	}
}

strings *split(char *line, char delimiter) {
	if (line == NULL) {
		return NULL;
	}
	int len = strlen(line);
	//printf("%d\n", len);
	
	strings *ss = create_strings();
	if (ss == NULL) {
		return NULL;
	}

	int i = 0, j = 0, ret = 0;;
	while (i < len) {
		//skip to blank char.
		while (i < len && isspace(line[i])) {
			++i;
		}	

		//add a string to store chars.
		ret = add_string(ss);
		//printf("line: %d, ret: %d\n", __LINE__, ret);
		if (ret == -1) {
			goto ERROR_RET;
		}
		
		//copy chars until encounter a delimiter or consume all chars. 
		while (i < len && line[i] != delimiter) {
			ret = append_char(ss, j, line[i]);
			//printf("line: %d, ret: %d\n", __LINE__, ret);
			if (ret == -1) {
				goto ERROR_RET;
			}
			++i;
		}
		//move string cursor to next.
		++j;

		//check whether the line ends with ",\0".
		//append an empty string in this situation.
		if (line[i] == delimiter && i + 1 == len) {
			ret = add_string(ss);
			//printf("line: %d, ret: %d\n", __LINE__, ret);
			if (ret == -1) {
				goto ERROR_RET;
			}

		}
		//skip to delimiter or move to next.
		++i;
	}

	return ss;

ERROR_RET:
	free_strings(&ss);
	return NULL;
}

int deal_server_config_line(char *line, sftt_server_config *ssc) {
	strip(line);
	//printf("stripped line: %s\n", line);
	int len = strlen(line);
	if (len < 1) {
		return -1;
	}

	char config_name[CONFIG_NAME_MAX_LEN];
	char config_value[CONFIG_VALUE_MAX_LEN];

	int i = 0, j = 0;
	while (i < len && j < CONFIG_NAME_MAX_LEN && line[i] != ' ') {
		config_name[j++] = line[i++];
	}
	if (i >= len || j >= CONFIG_NAME_MAX_LEN) {
		return -1;
	}
	config_name[j] = 0;	
	//printf("config name: %s\n", config_name);
	
	while (i < len && line[i] == ' ') {
		++i;
	}  
	if (i >= len || line[i] != '=') {
		return -1;
	} 

	++i;
	while (i < len && line[i] == ' ') {
		++i;
	}
	if (i >= len) {
		return -1;
	}

	j = 0;
	while (i < len && j < CONFIG_VALUE_MAX_LEN && line[i] != ' ') {			
		config_value[j++] = line[i++];	
	}
	if (j >= CONFIG_VALUE_MAX_LEN) {
		return -1;
	}
	config_value[j] = 0;
	//printf("config value: %s\n", config_value);
		
	if (strcmp(config_name, "store_path") == 0) {
		strncpy(ssc->store_path, config_value, DIR_PATH_MAX_LEN);
	} else if (strcmp(config_name, "block_size") == 0) {
		ssc->block_size = atoi(config_value);
	} else if (strcmp(config_name, "update_threshold") == 0) {
		ssc->update_th = atoi(config_value);
	} else if (strcmp(config_name, "log_dir") == 0) {
		strncpy(ssc->log_dir, config_value, DIR_PATH_MAX_LEN);
	}
	
	return 0;
}

int get_sftt_server_config(sftt_server_config *ssc) {
	if (ssc == NULL) {
		return -1;
	}
/*
	char buf[80];   
        getcwd(buf,sizeof(buf));   
      	printf("current working directory: %s\n", buf);   
*/
	char *server_config_path = "/etc/sftt/sftt_server.conf"; 
	FILE *fp = fopen(server_config_path, "r");
	if (fp == NULL) {
		printf("open config file failed: %s\n", server_config_path);
		goto ERR_RET;	
	}

	char line[CONFIG_LINE_MAX_SIZE];
	char *ret = NULL;
	int len = 0;
	int i = 0;
	while (!feof(fp)) {
		ret = fgets(line, CONFIG_LINE_MAX_SIZE, fp);
		//printf("%d: %s\n", i, line);
		//++i;
		if (ret == NULL) {
			//printf("get empty config line!\n");
			//break;
			continue ;
		}

		len = strlen(line);
		if (len < 1) {
			break;
		}

		if (line[0] == '#') {
			continue;
		}	
		
		if (line[len - 1] == '\n') {
			line[--len] = 0;
		}
		
		deal_server_config_line(line, ssc);
	}

	fclose(fp);

	return 0;

ERR_RET:
	printf("server config file parse failed!\n");

	return -1;
}

int deal_client_config_line(char *line, sftt_client_config *scc) {
	strip(line);
	//printf("stripped line: %s\n", line);
	int len = strlen(line);
	if (len < 1) {
		return -1;
	}

	char config_name[CONFIG_NAME_MAX_LEN];
	char config_value[CONFIG_VALUE_MAX_LEN];

	int i = 0, j = 0;
	while (i < len && j < CONFIG_NAME_MAX_LEN && line[i] != ' ') {
		config_name[j++] = line[i++];
	}
	if (i >= len || j >= CONFIG_NAME_MAX_LEN) {
		return -1;
	}
	config_name[j] = 0;	
	//printf("config name: %s\n", config_name);
	
	while (i < len && line[i] == ' ') {
		++i;
	}  
	if (i >= len || line[i] != '=') {
		return -1;
	} 

	++i;
	while (i < len && line[i] == ' ') {
		++i;
	}
	if (i >= len) {
		return -1;
	}

	j = 0;
	while (i < len && j < CONFIG_VALUE_MAX_LEN && line[i] != ' ') {			
		config_value[j++] = line[i++];	
	}
	if (j >= CONFIG_VALUE_MAX_LEN) {
		return -1;
	}
	config_value[j] = 0;
	//printf("config value: %s\n", config_value);
		
	if (strcmp(config_name, "block_size") == 0) {
		scc->block_size = atoi(config_value);
	} else if (strcmp(config_name, "log_dir") == 0) {
	    strncpy(scc->log_dir, config_value, DIR_PATH_MAX_LEN);
	}
	
	return 0;
}


int get_sftt_client_config(sftt_client_config *scc) {
	if (scc == NULL) {
		return -1;
	}
/*
	char buf[80];   
        getcwd(buf,sizeof(buf));   
      	printf("current working directory: %s\n", buf);   
*/
	char *client_config_path = "/etc/sftt/sftt_client.conf";
	FILE *fp = fopen(client_config_path, "r");
	if (fp == NULL) {
		printf("open config file failed: %s\n", client_config_path);
		goto ERR_RET;	
	}

	char line[CONFIG_LINE_MAX_SIZE];
	char *ret = NULL;
	int len = 0;
	int i = 0;
	while (!feof(fp)) {
		ret = fgets(line, CONFIG_LINE_MAX_SIZE, fp);
		//printf("%d: %s\n", i, line);
		//++i;
		if (ret == NULL) {
			//printf("get empty config line!\n");
			//break;
			continue ;
		}

		len = strlen(line);
		if (len < 1) {
			break;
		}

		if (line[0] == '#') {
			continue;
		}	
		
		if (line[len - 1] == '\n') {
			line[--len] = 0;
		}
		
		deal_client_config_line(line, scc);
	}

	fclose(fp);

	return 0;

ERR_RET:
	printf("client config file parse failed!\n");

	return -1;
}

#if 0
int main(void) {
	char *line = "a,0,0,,";
	int i = 0;
	
	strings *ss = split(line, ',');
	int num = get_string_num(ss);
	printf("num: %d\n", num);
	for (i = 0; i < num; ++i) {
		printf("%s\n", get_string(ss, i));
	}
	
	return 0;
}
#endif
