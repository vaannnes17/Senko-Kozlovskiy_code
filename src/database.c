#include "database.h"
#include "config.h"
#include <stdio.h>

int db_open(sqlite3 **db) {
    int rc = sqlite3_open(DB_PATH, db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(*db));
        return 0;
    }
    return 1;
}

void db_close(sqlite3 *db) {
    if (db) {
        sqlite3_close(db);
    }
}

int db_execute(sqlite3 *db, const char *sql, char **err_msg) {
    return sqlite3_exec(db, sql, 0, 0, err_msg);
}

long long db_last_insert_rowid(sqlite3 *db) {
    return sqlite3_last_insert_rowid(db);
}
