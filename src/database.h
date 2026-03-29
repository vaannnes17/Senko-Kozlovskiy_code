#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>

int db_open(sqlite3 **db);
void db_close(sqlite3 *db);
int db_execute(sqlite3 *db, const char *sql, char **err_msg);
long long db_last_insert_rowid(sqlite3 *db);

#endif
