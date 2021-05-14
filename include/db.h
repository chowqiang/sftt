#ifndef _DB_H_
#define _DB_H_

#include <sqlite3.h>

struct database {
	
};

struct db_connect {
	sqlite3 *db;
};

struct database *create_db(char *db_name);
struct db_connect *create_db_connect(char *db_name);
void destory_db_connect(struct db_connect *db_con);
int create_table(struct database *db, char *table_name);
int insert(struct database *db, char *table_name, void *data); 

#endif
