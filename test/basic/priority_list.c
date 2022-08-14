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

#include <sys/types.h>
#include <stdio.h>
#include "priority_list.h"

struct test_process {
	char *name;
	pid_t pid;
	struct priority_list_head list;
};

struct test_ctx {
	char *name;
	struct priority_list_head list;
};

int basic_test(int argc, char *argv[])
{
	struct test_process *tp;

	struct test_ctx ctx = {
		.name = "test_context"
	};
	PRIORITY_INIT_LIST_HEAD(&ctx.list, -1);

	struct test_process p1 = {
		.name = "server",
		.pid = 0,
	};
	PRIORITY_INIT_LIST_HEAD(&p1.list, 0);

	struct test_process p2 = {
		.name = "client1",
		.pid = 1,
	};
	PRIORITY_INIT_LIST_HEAD(&p2.list, 1);

	struct test_process p3 = {
		.name = "client2",
		.pid = 2,
	};
	PRIORITY_INIT_LIST_HEAD(&p3.list, 2);

	struct test_process p4 = {
		.name = "client3",
		.pid = 3,
	};
	PRIORITY_INIT_LIST_HEAD(&p4.list, 3);

	struct test_process p5 = {
		.name = "client4",
		.pid = 4,
	};
	PRIORITY_INIT_LIST_HEAD(&p5.list, 1);

	struct test_process p6 = {
		.name = "client5",
		.pid = 5,
	};
	PRIORITY_INIT_LIST_HEAD(&p6.list, 2);

	struct test_process p7 = {
		.name = "client6",
		.pid = 6,
	};
	PRIORITY_INIT_LIST_HEAD(&p7.list, 3);


	priority_list_add(&p5.list, &ctx.list);
	priority_list_add(&p2.list, &ctx.list);
	priority_list_add(&p1.list, &ctx.list);
	priority_list_add(&p3.list, &ctx.list);
	priority_list_add(&p4.list, &ctx.list);
	priority_list_add(&p6.list, &ctx.list);
	priority_list_add(&p7.list, &ctx.list);

	priority_list_for_each_entry(tp, &ctx.list, list)
		printf("name = %s, pid = %d, priority = %d\n",
				tp->name, tp->pid, get_priority(&tp->list));
	return 0;
}

int main(int argc, char *argv[])
{
	basic_test(argc, argv);

	return 0;
}
