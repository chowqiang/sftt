#ifndef _DB_H_
#define _DB_H_

#include <sqlite3.h>
#include "map.h"

struct db_connect {
	sqlite3 *db;
};

struct database *create_db(char *db_name);
struct db_connect *create_db_connect(char *db_file);
void destory_db_connect(struct db_connect *db_con);
int create_table(struct db_connect *db_con, char *table_name);
int db_insert(struct db_connect *db_con, char *sql, char **err_msg);
int db_select(struct db_connect *db_con, char *sql, struct map **data, char **err_msg);
int db_select_count(struct db_connect *db_con, char *sql, char **err_msg);

#endif
