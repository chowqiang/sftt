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

#ifndef _PRIORITY_LIST_H_
#define _PRIORITY_LIST_H_

#include <stddef.h>
#include "list.h"

struct priority_list_head {
	int priority;
	struct list_head list;
};

#define PRIORITY_LIST_HEAD_INIT(name, priority) \
	{.priority = priority, .list = LIST_HEAD_INIT(name)}

#define PRIORITY_LIST_HEAD(name, priority) \
	struct priority_list_head name = PRIORITY_LIST_HEAD_INIT(name, \
			priority)

static inline void
PRIORITY_INIT_LIST_HEAD(struct priority_list_head *list, int priority)
{
	list->priority = priority;
	INIT_LIST_HEAD(&list->list);
}

static inline void
priority_list_add(struct priority_list_head *entry, struct priority_list_head *head)
{
	struct priority_list_head *pos = NULL, *pre = NULL;

	list_for_each_entry(pos, &head->list, list) {
		if (pos->priority > entry->priority)
			break;
		pre = pos;
	}
	if (pre == NULL) {
		list_add(&entry->list, &head->list);
	} else {
		list_add(&entry->list, &pre->list);
	}
}

static inline void
priority_list_del(struct priority_list_head *entry)
{
	list_del(&entry->list);
}

static inline bool
priority_list_empty(struct priority_list_head *head)
{
	return list_empty(&head->list);
}

static inline int
get_priority(struct priority_list_head *entry)
{
	return entry->priority;
}

#ifndef priority_container_of
#define priority_container_of(ptr, type, member) \
	(type *)((char *)(ptr) - (char *) &((type *) 0)->member)
#endif

#define priority_list_entry(ptr, type, member) \
	priority_container_of(ptr, type, member)

#define priority_list_first_entry(ptr, type, member) \
	priority_list_entry( \
		container_of((ptr)->list->next, \
			struct priority_list_head, \
			list), \
		type, \
		member)

#define priority_list_last_entry(ptr, type, member) \
	priority_list_entry( \
		container_of((ptr)->list->prev, \
			struct priority_list_head, \
			list), \
		type, \
		member)

#define priority_list_for_each_entry(pos, head, member) \
	for (pos = priority_container_of( \
			container_of((head)->list.next, \
				struct priority_list_head, \
				list), \
			typeof(*(pos)), \
			member); \
	     &pos->member != (head); \
	     pos = priority_container_of( \
			container_of(pos->member.list.next, \
				struct priority_list_head, \
				list), \
			typeof(*(pos)), \
			member))
#endif
