#include "driver.h"
#include "database.h"
#include <stdio.h>
#include <string.h>

int driver_create_table(sqlite3 *db) {
    char *err_msg = 0;
    const char *sql = 
        "CREATE TABLE IF NOT EXISTS drivers ("
        "driver_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "personnel_number TEXT UNIQUE NOT NULL,"
        "last_name TEXT NOT NULL,"
        "first_name TEXT NOT NULL,"
        "patronymic TEXT,"
        "category TEXT NOT NULL,"
        "experience INTEGER DEFAULT 0,"
        "address TEXT,"
        "birth_year INTEGER"
        ");";
    
    int rc = db_execute(db, sql, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return 0;
    }
    
    const char *sql_stats = 
        "CREATE TABLE IF NOT EXISTS driver_stats ("
        "driver_id INTEGER PRIMARY KEY,"
        "total_trips INTEGER DEFAULT 0,"
        "total_cargo REAL DEFAULT 0,"
        "total_earnings REAL DEFAULT 0,"
        "FOREIGN KEY (driver_id) REFERENCES drivers(driver_id) ON DELETE CASCADE"
        ");";
    
    rc = db_execute(db, sql_stats, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return 0;
    }
    
    printf("Tables created successfully\n");
    return 1;
}

int driver_insert(sqlite3 *db, const Driver *d) {
    char sql[1024];
    char *err_msg = 0;
    
    sprintf(sql, 
        "INSERT INTO drivers (personnel_number, last_name, first_name, patronymic, "
        "category, experience, address, birth_year) "
        "VALUES ('%s', '%s', '%s', '%s', '%s', %d, '%s', %d);",
        d->personnel_number, d->last_name, d->first_name, d->patronymic,
        d->category, d->experience, d->address, d->birth_year);
    
    int rc = db_execute(db, sql, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Insert error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return 0;
    }
    
    long long driver_id = db_last_insert_rowid(db);
    char sql_stats[256];
    sprintf(sql_stats, "INSERT INTO driver_stats (driver_id) VALUES (%lld);", driver_id);
    db_execute(db, sql_stats, &err_msg);
    
    return 1;
}

int driver_select_by_id(sqlite3 *db, int id, Driver *d) {
    sqlite3_stmt *stmt;
    char sql[256];
    sprintf(sql, "SELECT driver_id, personnel_number, last_name, first_name, patronymic, "
                 "category, experience, address, birth_year FROM drivers WHERE driver_id = ?");
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) return 0;
    
    sqlite3_bind_int(stmt, 1, id);
    rc = sqlite3_step(stmt);
    
    if (rc == SQLITE_ROW) {
        d->id = sqlite3_column_int(stmt, 0);
        strcpy(d->personnel_number, (const char*)sqlite3_column_text(stmt, 1));
        strcpy(d->last_name, (const char*)sqlite3_column_text(stmt, 2));
        strcpy(d->first_name, (const char*)sqlite3_column_text(stmt, 3));
        const char *patr = (const char*)sqlite3_column_text(stmt, 4);
        if (patr) strcpy(d->patronymic, patr);
        else d->patronymic[0] = '\0';
        strcpy(d->category, (const char*)sqlite3_column_text(stmt, 5));
        d->experience = sqlite3_column_int(stmt, 6);
        strcpy(d->address, (const char*)sqlite3_column_text(stmt, 7));
        d->birth_year = sqlite3_column_int(stmt, 8);
        
        printf("\n=== Driver found ===\n");
        printf("ID: %d\n", d->id);
        printf("Personnel number: %s\n", d->personnel_number);
        printf("Name: %s %s %s\n", d->last_name, d->first_name, d->patronymic);
        printf("Category: %s\n", d->category);
        printf("Experience: %d years\n", d->experience);
        printf("Address: %s\n", d->address);
        printf("Birth year: %d\n", d->birth_year);
        
        sqlite3_finalize(stmt);
        return 1;
    }
    
    sqlite3_finalize(stmt);
    return 0;
}

int driver_select_by_lastname(sqlite3 *db, const char *lastname) {
    sqlite3_stmt *stmt;
    char sql[256];
    sprintf(sql, "SELECT driver_id, personnel_number, last_name, first_name, patronymic, "
                 "category, experience, address, birth_year FROM drivers WHERE last_name LIKE ?");
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) return 0;
    
    char pattern[100];
    sprintf(pattern, "%s%%", lastname);
    sqlite3_bind_text(stmt, 1, pattern, -1, SQLITE_STATIC);
    
    printf("\n=== Drivers with last name starting with '%s' ===\n", lastname);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        printf("%d | %s | %s %s %s | %s | Exp: %d\n",
            sqlite3_column_int(stmt, 0),
            sqlite3_column_text(stmt, 1),
            sqlite3_column_text(stmt, 2),
            sqlite3_column_text(stmt, 3),
            sqlite3_column_text(stmt, 4) ? (const char*)sqlite3_column_text(stmt, 4) : "",
            sqlite3_column_text(stmt, 5),
            sqlite3_column_int(stmt, 6));
    }
    
    sqlite3_finalize(stmt);
    return 1;
}

int driver_select_all(sqlite3 *db) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT driver_id, personnel_number, last_name, first_name, patronymic, "
                      "category, experience, address, birth_year FROM drivers";
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) return 0;
    
    printf("\n=== All drivers ===\n");
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        printf("%d | %s | %s %s %s | %s | Exp: %d | %d\n",
            sqlite3_column_int(stmt, 0),
            sqlite3_column_text(stmt, 1),
            sqlite3_column_text(stmt, 2),
            sqlite3_column_text(stmt, 3),
            sqlite3_column_text(stmt, 4) ? (const char*)sqlite3_column_text(stmt, 4) : "",
            sqlite3_column_text(stmt, 5),
            sqlite3_column_int(stmt, 6),
            sqlite3_column_int(stmt, 8));
    }
    
    sqlite3_finalize(stmt);
    return 1;
}

int driver_update(sqlite3 *db, int id, int new_experience) {
    char sql[256];
    char *err_msg = 0;
    sprintf(sql, "UPDATE drivers SET experience = %d WHERE driver_id = %d;", new_experience, id);
    
    int rc = db_execute(db, sql, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Update error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return 0;
    }
    return 1;
}

int driver_delete(sqlite3 *db, int id) {
    char sql[256];
    char *err_msg = 0;
    sprintf(sql, "DELETE FROM drivers WHERE driver_id = %d;", id);
    
    int rc = db_execute(db, sql, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Delete error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return 0;
    }
    return 1;
}
