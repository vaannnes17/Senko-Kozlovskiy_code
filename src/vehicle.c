#include "vehicle.h"
#include "database.h"
#include <stdio.h>
#include <string.h>

int vehicle_create_table(sqlite3 *db) {
    char *err_msg = 0;
    const char *sql = 
        "CREATE TABLE IF NOT EXISTS vehicles ("
        "vehicle_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "plate_number TEXT UNIQUE NOT NULL,"
        "brand TEXT NOT NULL,"
        "mileage_on_purchase INTEGER DEFAULT 0,"
        "capacity REAL NOT NULL"
        ");";
    
    int rc = db_execute(db, sql, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return 0;
    }
    
    const char *sql_stats = 
        "CREATE TABLE IF NOT EXISTS vehicle_stats ("
        "vehicle_id INTEGER PRIMARY KEY,"
        "total_mileage INTEGER DEFAULT 0,"
        "total_cargo REAL DEFAULT 0,"
        "total_trips INTEGER DEFAULT 0,"
        "FOREIGN KEY (vehicle_id) REFERENCES vehicles(vehicle_id) ON DELETE CASCADE"
        ");";
    
    rc = db_execute(db, sql_stats, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return 0;
    }
    
    return 1;
}

int vehicle_insert(sqlite3 *db, const Vehicle *v) {
    char sql[1024];
    char *err_msg = 0;
    
    sprintf(sql, 
        "INSERT INTO vehicles (plate_number, brand, mileage_on_purchase, capacity) "
        "VALUES ('%s', '%s', %d, %.2f);",
        v->plate_number, v->brand, v->mileage_on_purchase, v->capacity);
    
    int rc = db_execute(db, sql, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Insert error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return 0;
    }
    
    long long vehicle_id = db_last_insert_rowid(db);
    char sql_stats[256];
    sprintf(sql_stats, "INSERT INTO vehicle_stats (vehicle_id) VALUES (%lld);", vehicle_id);
    db_execute(db, sql_stats, &err_msg);
    
    return 1;
}

int vehicle_select_by_id(sqlite3 *db, int id, Vehicle *v) {
    sqlite3_stmt *stmt;
    char sql[256];
    sprintf(sql, "SELECT vehicle_id, plate_number, brand, mileage_on_purchase, capacity "
                 "FROM vehicles WHERE vehicle_id = ?");
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) return 0;
    
    sqlite3_bind_int(stmt, 1, id);
    rc = sqlite3_step(stmt);
    
    if (rc == SQLITE_ROW) {
        v->id = sqlite3_column_int(stmt, 0);
        strcpy(v->plate_number, (const char*)sqlite3_column_text(stmt, 1));
        strcpy(v->brand, (const char*)sqlite3_column_text(stmt, 2));
        v->mileage_on_purchase = sqlite3_column_int(stmt, 3);
        v->capacity = sqlite3_column_double(stmt, 4);
        
        printf("\n=== Vehicle found ===\n");
        printf("ID: %d\n", v->id);
        printf("Plate number: %s\n", v->plate_number);
        printf("Brand: %s\n", v->brand);
        printf("Mileage on purchase: %d km\n", v->mileage_on_purchase);
        printf("Capacity: %.2f tons\n", v->capacity);
        
        sqlite3_finalize(stmt);
        return 1;
    }
    
    sqlite3_finalize(stmt);
    return 0;
}

int vehicle_select_all(sqlite3 *db) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT vehicle_id, plate_number, brand, mileage_on_purchase, capacity FROM vehicles";
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) return 0;
    
    printf("\n=== All vehicles ===\n");
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        printf("%d | %s | %s | Mileage: %d | Capacity: %.2f\n",
            sqlite3_column_int(stmt, 0),
            sqlite3_column_text(stmt, 1),
            sqlite3_column_text(stmt, 2),
            sqlite3_column_int(stmt, 3),
            sqlite3_column_double(stmt, 4));
    }
    
    sqlite3_finalize(stmt);
    return 1;
}

int vehicle_update(sqlite3 *db, int id, int new_mileage) {
    char sql[256];
    char *err_msg = 0;
    sprintf(sql, "UPDATE vehicles SET mileage_on_purchase = %d WHERE vehicle_id = %d;", new_mileage, id);
    
    int rc = db_execute(db, sql, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Update error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return 0;
    }
    return 1;
}

int vehicle_delete(sqlite3 *db, int id) {
    char sql[256];
    char *err_msg = 0;
    sprintf(sql, "DELETE FROM vehicles WHERE vehicle_id = %d;", id);
    
    int rc = db_execute(db, sql, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Delete error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return 0;
    }
    return 1;
}

double vehicle_get_capacity(sqlite3 *db, int id) {
    sqlite3_stmt *stmt;
    char sql[256];
    sprintf(sql, "SELECT capacity FROM vehicles WHERE vehicle_id = ?");
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) return -1;
    
    sqlite3_bind_int(stmt, 1, id);
    rc = sqlite3_step(stmt);
    
    if (rc == SQLITE_ROW) {
        double capacity = sqlite3_column_double(stmt, 0);
        sqlite3_finalize(stmt);
        return capacity;
    }
    
    sqlite3_finalize(stmt);
    return -1;
}
