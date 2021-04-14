#ifndef _DB_H_
#define _DB_H_

struct database {
	
};

struct db_connect {

};

struct database *create_db(char *db_name);
struct db_connect *create_db_connect(char *db_name);
int create_table(struct database *db, char *table_name);
int insert(struct database *db, char *table_name, void *data); 

#endif
