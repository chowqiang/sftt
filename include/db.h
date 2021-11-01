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

#ifndef _DB_H_
#define _DB_H_

#include <sqlite3.h>
#include "map.h"

struct db_connect {
	sqlite3 *db;
};

#define col_fmt(type)           \
({                              \
	const char *fmt = "%s"; \
                                \
	switch (type) {         \
	case INTEGER:           \
		fmt = "%d";     \
	break;                  \
	case REAL:              \
		fmt = "%lf";    \
	break;                  \
	case TEXT:              \
		fmt = "%s";     \
	break;                  \
	default:                \
	break;                  \
	}                       \
                                \
	fmt;                    \
})

#define col_value(type, ptr)                           \
({                                                     \
	unsigned long __value = (unsigned long)ptr;    \
                                                       \
	switch (type) {                                \
	case INTEGER:                                  \
		__value = atoi(ptr);                   \
		break;                                 \
	break;                                         \
	case REAL:                                     \
		__value = atof(ptr);                   \
	break;                                         \
	case TEXT:                                     \
		__value = (unsigned long)ptr;          \
	break;                                         \
	default:                                       \
	break;                                         \
	}                                              \
                                                       \
	__value;                                       \
})

enum col_type {
	INTEGER,
	REAL,
	TEXT,
	UNKNOWN_COL_TYPE,
};

struct col_info {
	const char *name;
	enum col_type type;
};

struct table_info {
	struct col_info *cols;
	int nr_cols;
};

struct database *create_db(char *db_name);
struct db_connect *create_db_connect(char *db_file);
void destroy_db_connect(struct db_connect *db_con);
int create_table(struct db_connect *db_con, char *table_name);
int db_insert(struct db_connect *db_con, char *sql, char **err_msg);
int db_update(struct db_connect *db_con, char *sql, char **err_msg);
int db_select(struct db_connect *db_con, char *sql, struct map **data, char **err_msg);
int db_select_count(struct db_connect *db_con, char *sql, char **err_msg);
enum col_type get_col_type(struct table_info *table, char *col_name);
#endif
