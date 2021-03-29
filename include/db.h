#ifndef _DB_H_
#define _DB_H_

typedef struct {
	
} database;

typedef struct {

} db_connect;

database *create_db(char *db_name);
db_connect *create_db_connect(char *db_name);
int create_table(database *db, char *table_name);
int insert(database *db, char *table_name, void *data); 

#endif
