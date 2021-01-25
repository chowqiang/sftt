#include <stdio.h>
#include <sqlite3.h>

int test2(int argc, char* argv[]) {
   sqlite3 *db;
   char *zErrMsg = 0;
   int rc;

   rc = sqlite3_open("test.db", &db);

   if( rc ) {
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      return(0);
   } else {
      fprintf(stderr, "Opened database successfully\n");
   }
   sqlite3_close(db);
}

void test(int argc, char *argv[]) {
    test2(argc, argv);
}

int main(int argc, char *argv[]) {
    test(argc, argv);

    return 0;
}

