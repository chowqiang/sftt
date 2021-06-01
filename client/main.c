#include <stdio.h>
#include "client.h"
#include "cmdline.h"
#include "log.h"

extern struct sftt_option sftt_client_opts[];
int main(int argc, char **argv) {
	int optind = 1;
	char *optarg = NULL;
	bool has_passwd_opt = false;
	char user_name[USER_NAME_MAX_LEN];
	char password[PASSWD_MD5_LEN];
	char host[HOST_MAX_LEN];
	char passwd_prompt[128];
	int port = -1;
	const struct sftt_option *opt = NULL;	
	bool ret = false;
	int passwd_len = 0;

	memset(user_name, 0, sizeof(user_name));
	memset(password, 0, sizeof(password));
	memset(host, 0, sizeof(host));

	if (argc == 2 && try_fetch_login_info(argv[1], user_name, host) == 0) {
		has_passwd_opt = true;
		goto input_passwd;
	}

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
			ret = parse_user_name(optarg, user_name, sizeof(user_name));
			if (!ret) {
				printf("user name is invalid!\n");	
				client_usage_help(-1);
			}
			break;
		case HOST:
			ret = parse_host(optarg, host, sizeof(host));
			if (!ret) {
				printf("host is invalid!\n");
				client_usage_help(-1);
			}
			break;
		case PORT:
			ret = parse_port(optarg, &port);
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
	if (strlen(user_name) == 0) {
		printf("user name is invalid!\n");
		client_usage_help(-1);
	}

	if (strlen(host) == 0) {
		printf("host is invalid!\n");
		client_usage_help(-1);
	}

input_passwd:
	snprintf(passwd_prompt, 127, "%s@%s's password: ", user_name, host);
	if (has_passwd_opt) {
		passwd_len = get_pass(passwd_prompt, password, sizeof(password));
		if (passwd_len <= 0) {
			printf("password is invalid!\n");
			client_usage_help(-1);
		}
	}

#ifdef DEBUG_ENABLE
	show_options(host, user_name, password);
#endif

	struct sftt_client_v2 client;
	if (init_sftt_client_v2(&client, host, port, user_name, password) == -1) {
		printf("init sftt client failed!\n");
		exit(-1);
	}


	add_log(LOG_INFO, "client validate successfully!");

	reader_loop2(&client);

	return 0;
}
