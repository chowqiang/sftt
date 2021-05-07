#include <stdio.h>
#include "client.h"
#include "log.h"

extern struct sftt_option sftt_client_opts[];
int main(int argc, char **argv) {
	int optind = 1;
	char *optarg = NULL;
	bool has_passwd_opt = false;
	char user_name[USER_NAME_MAX_LEN];
	char password[PASSWD_MAX_LEN];
	char host[HOST_MAX_LEN];
	int port = -1;
	const struct sftt_option *opt = NULL;	
	bool ret = false;
	int passwd_len = 0;

	memset(user_name, 0, sizeof(user_name));
	memset(password, 0, sizeof(password));
	memset(host, 0, sizeof(host));
	for (;;) {
		if (optind >= argc) {
			break;
		}
		opt = lookup_opt(argc, argv, &optarg, &optind, sftt_client_opts);
		if (opt == NULL) {
			printf("invalid option\n");
			client_usage_help(-1);
		}
		switch (opt->index) {
		case USER:
			ret = user_name_parse(optarg, user_name, sizeof(user_name));
			if (!ret) {
				printf("user name is invalid!\n");	
				client_usage_help(-1);
			}
			break;
		case HOST:
			ret = host_parse(optarg, host, sizeof(host));
			if (!ret) {
				printf("host is invalid!\n");
				client_usage_help(-1);
			}
			break;
		case PORT:
			ret = port_parse(optarg, &port);
			if (!ret) {
				printf("port is invalid!\n");
				client_usage_help(-1);
			}
			break;
		case PASSWORD:
			has_passwd_opt = true;
			break;
		}
	}

	if (has_passwd_opt) {
		passwd_len = get_pass("password: ", password, sizeof(password));
		if (passwd_len <= 0) {
			printf("password is invalid!\n");
			client_usage_help(-1);
		}
	}

	if (strlen(user_name) == 0) {
		printf("user name is invalid!\n");
		client_usage_help(-1);
	}

	if (strlen(host) == 0) {
		printf("host is invalid!\n");
		client_usage_help(-1);
	}

#ifdef DEBUG_ENABLE
	show_options(host, user_name, password);
#endif

	struct sftt_client_v2 client;
	if (init_sftt_client_v2(&client, host, port, user_name) == -1) {
		printf("init sftt client failed!\n");
		exit(-1);
	}

	if (validate_user_info(&client, password) == -1) {
		printf("cannot validate user and password!\n");
		exit(-1);
	}

	add_log(LOG_INFO, "client validate successfully!");

	reader_loop2(&client);

	return 0;
}
