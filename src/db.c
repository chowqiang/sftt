#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "db.h"
#include "mem_pool.h"

extern struct mem_pool *g_mp;

struct database *create_db(char *db_name) {
	return NULL;
}

struct db_connect *create_db_connect(char *db_name) {
	sqlite3 *db;
	int rc;
	struct db_connect *db_con;

	db_con = mp_malloc(g_mp, sizeof(struct db_connect));
	assert(db_con != NULL);

	rc = sqlite3_open(db_name, &db);
	if (rc) {
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		return NULL;
	}
	//else {
	//	fprintf(stderr, "Opened database successfully\n");
	//}

	db_con->db = db;

	return db_con;
}

void destory_db_connect(struct db_connect *db_con)
{
	if (db_con == NULL || db_con->db == NULL)
		return ;

	sqlite3_close(db_con->db);
}

int create_table(struct database *db, char *table_name) {
	return 0;
}

int insert(struct database *db, char *table_name, void *data) {
	return 0;
}
