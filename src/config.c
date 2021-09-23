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

#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "config.h"
#include "file.h"
#include "mem_pool.h"
#include "req_resp.h"
#include "utils.h"

extern struct mem_pool *g_mp;

char *config_search_pathes[] = {"/etc/sftt", "./config", ".",
	"/root/sftt/config", NULL};

char *search_config(char *fname)
{
	int i;
	char tmp[256];

	for (i = 0; config_search_pathes[i] != NULL; ++i) {
		snprintf(tmp, 255, "%s/%s", config_search_pathes[i], fname);
		if (file_existed(tmp))
			return __strdup(tmp);
	}

	return NULL;
}

int deal_server_config_line(char *line, struct sftt_server_config *ssc)
{
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

int get_sftt_server_config(struct sftt_server_config *ssc)
{
	if (ssc == NULL) {
		return -1;
	}
/*
	char buf[80];   
        getcwd(buf,sizeof(buf));   
      	printf("current working directory: %s\n", buf);   
*/
	char *server_config_path;
	if ((server_config_path	= search_config("sftt_server.conf")) == NULL) {
		printf("cannot find server config file!\n");
		return -1;
	}

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
	mp_free(g_mp, server_config_path);

	return 0;

ERR_RET:
	printf("server config file parse failed!\n");
	mp_free(g_mp, server_config_path);

	return -1;
}

int deal_client_config_line(char *line, struct sftt_client_config *scc)
{
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


int get_sftt_client_config(struct sftt_client_config *scc)
{
	if (scc == NULL) {
		return -1;
	}
/*
	char buf[80];   
        getcwd(buf,sizeof(buf));   
      	printf("current working directory: %s\n", buf);   
*/
	char *client_config_path;
	if ((client_config_path	= search_config("sftt_client.conf")) == NULL) {
		printf("cannot find client config file!\n");
		return -1;
	}

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
	mp_free(g_mp, client_config_path);

	return 0;

ERR_RET:
	printf("client config file parse failed!\n");
	mp_free(g_mp, client_config_path);

	return -1;
}
