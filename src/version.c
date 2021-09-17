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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "version.h"

void show_version(void)
{
	printf("version " VERSION ", Copyright (c) 2020-2021 zhou min, zhou qiang\n");
}

short get_short(char *p, int len)
{
	int i;
	short num = 0;

	if (len <= 0)
		return -1;

	for (i = 0; i < len; ++i)
		if (!(p[i] >= '0' && p[i] <= '9'))
			return -1;

	for (i = 0; i < len; ++i)
		num = num * 10 + (p[i] - '0');

	return num;
}

int get_version_info(struct version_info *ver)
{
	int ret;
	short major, minor, revision;
	char *p, *q;

	p = VERSION;
	q = strchr(p, '.');
	if (q == NULL)
		return -1;

	major = get_short(p, q - p);
	if (major == -1)
		return -1;

	p = q + 1;
	q = strchr(p, '.');
	if (q == NULL)
		return -1;

	minor = get_short(p, q - p);
	if (minor == -1)
		return -1;

	p = q + 1;
	revision = atoi(p);

	ver->major = major;
	ver->minor = minor;
	ver->revision = revision;

	return 0;
}

int check_version(struct version_info *client, struct version_info *server,
	char *message, int len)
{
	if (client->major == server->major &&
		client->minor == server->minor) {
		if (server->major == 0 && server->minor == 0 &&
			client->revision != server->revision) {
			snprintf(message, len, "Version not matched, cannot connect!\n"
				"Client's version is %d.%d.%d, server's version is "
				"%d.%d.%d!\nPlease use version of %d.%d.%d for client!",
				client->major, client->minor, client->revision,
				server->major, server->minor, server->revision,
				server->major, server->minor, server->revision);
			return -1;
		}

	} else {
		if (server->major == 0 && server->minor == 0) {
			snprintf(message, len, "Version not matched, cannot connect!\n"
				"Client's version is %d.%d.%d, server's version is "
				"%d.%d.%d!\nPlease use version of %d.%d.%d for client!",
				client->major, client->minor, client->revision,
				server->major, server->minor, server->revision,
				server->major, server->minor, server->revision);

		} else {
			snprintf(message, len, "Version not matched, cannot connect!\n"
				"Client's version is %d.%d.%d, server's version is "
				"%d.%d.%d!\nPlease use version of %d.%d.xx for client!",
				client->major, client->minor, client->revision,
				server->major, server->minor, server->revision,
				server->major, server->minor);
		}

		return -1;
	}

	return 0;
}
