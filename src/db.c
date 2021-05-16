#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "db.h"
#include "map.h"
#include "mem_pool.h"

extern struct mem_pool *g_mp;

struct database *create_db(char *db_name) {
	return NULL;
}

struct db_connect *create_db_connect(char *db_file) {
	sqlite3 *db;
	int rc;
	struct db_connect *db_con;

	db_con = mp_malloc(g_mp, sizeof(struct db_connect));
	assert(db_con != NULL);

	rc = sqlite3_open(db_file, &db);
	if (rc != SQLITE_OK) {
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

int create_table(struct db_connect *db_con, char *table_name) {
	return 0;
}

int db_insert(struct db_connect *db_con, char *table_name, void *data) {
	return 0;
}

int fetch_select_count_from_result(void *pnum, int ret_cnt, char **cols_val, char **cols_name)
{
	if (cols_val == NULL || cols_val[0] == NULL) {
		printf("%s:%d, sql result bad!\n", __func__, __LINE__);
		*(int *)pnum = -1;

		return -1;
	}

	*(int *)pnum = atoi(cols_val[0]);

	return 0;
}

int db_select_count(struct db_connect *db_con, char *sql, char **err_msg)
{
	int count = -1;
	int ret;

	ret = sqlite3_exec(db_con->db, sql, fetch_select_count_from_result, &count, err_msg);
	if (ret != SQLITE_OK) {
		printf("%s:%d, sqlite3 exec failed!\n", __func__, __LINE__);
	}

	return count;
}

int db_select(struct db_connect *db_con, char *sql, struct map **data, char **err_msg)
{
	char **pret;
	int rows, cols;
	struct map *tmp;
	int i, j, ret;

	if (db_con == NULL || db_con->db == NULL || sql == NULL || data == NULL || err_msg == NULL) {
		printf("%s:%d, params error.\n", __func__, __LINE__);
		return -1;
	}

	sqlite3_get_table(db_con->db, sql, &pret, &rows, &cols, err_msg);
	if (pret == NULL || rows == 0 || cols == 0) {
		printf("%s:%d, cannot find records.\n", __func__, __LINE__);
		return -1;
	}

	printf("select result: rows = %d, cols = %d\n", rows, cols);

	tmp = mp_malloc(g_mp, rows * sizeof(struct map));
	assert(tmp != NULL);

	for (i = 1; i <= rows; ++i) {
		ret = map_init(&tmp[i - 1]);
		assert(ret == 0);

		for (j = 0; j < cols; ++j)
			if (map_add(&tmp[i - 1], pret[j], pret[i * cols + j]) == -1) {
				printf("cannot add %s to map\n", pret[j]);
			}
	}

	*data = tmp;

	return rows;
}
