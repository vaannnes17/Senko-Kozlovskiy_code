#include "order.h"
#include "database.h"
#include "vehicle.h"
#include <stdio.h>
#include <string.h>

int order_create_table(sqlite3 *db) {
    char *err_msg = 0;
    const char *sql = 
        "CREATE TABLE IF NOT EXISTS orders ("
        "order_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "order_date DATE NOT NULL,"
        "driver_id INTEGER NOT NULL,"
        "vehicle_id INTEGER NOT NULL,"
        "distance INTEGER,"
        "cargo_weight REAL,"
        "cost REAL,"
        "FOREIGN KEY (driver_id) REFERENCES drivers(driver_id) ON DELETE CASCADE,"
        "FOREIGN KEY (vehicle_id) REFERENCES vehicles(vehicle_id) ON DELETE CASCADE"
        ");";
    
    int rc = db_execute(db, sql, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return 0;
    }
    
    const char *sql_earnings = 
        "CREATE TABLE IF NOT EXISTS earnings ("
        "earning_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "driver_id INTEGER NOT NULL,"
        "order_id INTEGER UNIQUE NOT NULL,"
        "amount REAL,"
        "calculated_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
        "FOREIGN KEY (driver_id) REFERENCES drivers(driver_id) ON DELETE CASCADE,"
        "FOREIGN KEY (order_id) REFERENCES orders(order_id) ON DELETE CASCADE"
        ");";
    
    rc = db_execute(db, sql_earnings, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return 0;
    }
    
    const char *trigger = 
        "CREATE TRIGGER IF NOT EXISTS calculate_driver_earnings "
        "AFTER INSERT ON orders "
        "BEGIN "
        "INSERT INTO earnings (driver_id, order_id, amount) "
        "VALUES (NEW.driver_id, NEW.order_id, NEW.cost * 0.2); "
        "UPDATE driver_stats "
        "SET total_trips = total_trips + 1, "
        "total_cargo = total_cargo + NEW.cargo_weight, "
        "total_earnings = total_earnings + (NEW.cost * 0.2) "
        "WHERE driver_id = NEW.driver_id; "
        "UPDATE vehicle_stats "
        "SET total_mileage = total_mileage + NEW.distance, "
        "total_cargo = total_cargo + NEW.cargo_weight, "
        "total_trips = total_trips + 1 "
        "WHERE vehicle_id = NEW.vehicle_id; "
        "END;";
    
    rc = db_execute(db, trigger, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return 0;
    }
    
    return 1;
}

int order_check_capacity(sqlite3 *db, int vehicle_id, double cargo_weight) {
    double capacity = vehicle_get_capacity(db, vehicle_id);
    if (capacity < 0) {
        printf("Vehicle not found\n");
        return 0;
    }
    if (cargo_weight > capacity) {
        printf("Error: cargo weight (%.2f) exceeds vehicle capacity (%.2f)\n", cargo_weight, capacity);
        return 0;
    }
    return 1;
}

int order_insert(sqlite3 *db, const Order *o) {
    if (!order_check_capacity(db, o->vehicle_id, o->cargo_weight)) {
        return 0;
    }
    
    char sql[1024];
    char *err_msg = 0;
    
    sprintf(sql, 
        "INSERT INTO orders (order_date, driver_id, vehicle_id, distance, cargo_weight, cost) "
        "VALUES ('%s', %d, %d, %d, %.2f, %.2f);",
        o->order_date, o->driver_id, o->vehicle_id, o->distance, o->cargo_weight, o->cost);
    
    int rc = db_execute(db, sql, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Insert error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return 0;
    }
    
    return 1;
}

int order_select_by_driver(sqlite3 *db, int driver_id, const char *start_date, const char *end_date) {
    sqlite3_stmt *stmt;
    char sql[512];
    sprintf(sql, "SELECT order_id, order_date, vehicle_id, distance, cargo_weight, cost "
                 "FROM orders WHERE driver_id = ? AND order_date BETWEEN ? AND ?");
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) return 0;
    
    sqlite3_bind_int(stmt, 1, driver_id);
    sqlite3_bind_text(stmt, 2, start_date, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, end_date, -1, SQLITE_STATIC);
    
    printf("\n=== Orders for driver ID %d from %s to %s ===\n", driver_id, start_date, end_date);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        printf("Order ID: %d | Date: %s | Vehicle ID: %d | Distance: %d | Cargo: %.2f | Cost: %.2f\n",
            sqlite3_column_int(stmt, 0),
            sqlite3_column_text(stmt, 1),
            sqlite3_column_int(stmt, 2),
            sqlite3_column_int(stmt, 3),
            sqlite3_column_double(stmt, 4),
            sqlite3_column_double(stmt, 5));
    }
    
    sqlite3_finalize(stmt);
    return 1;
}

int order_select_by_vehicle(sqlite3 *db, int vehicle_id) {
    sqlite3_stmt *stmt;
    char sql[256];
    sprintf(sql, "SELECT order_id, order_date, driver_id, distance, cargo_weight, cost "
                 "FROM orders WHERE vehicle_id = ?");
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) return 0;
    
    sqlite3_bind_int(stmt, 1, vehicle_id);
    
    printf("\n=== Orders for vehicle ID %d ===\n", vehicle_id);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        printf("Order ID: %d | Date: %s | Driver ID: %d | Distance: %d | Cargo: %.2f | Cost: %.2f\n",
            sqlite3_column_int(stmt, 0),
            sqlite3_column_text(stmt, 1),
            sqlite3_column_int(stmt, 2),
            sqlite3_column_int(stmt, 3),
            sqlite3_column_double(stmt, 4),
            sqlite3_column_double(stmt, 5));
    }
    
    sqlite3_finalize(stmt);
    return 1;
}

int order_select_all(sqlite3 *db) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT order_id, order_date, driver_id, vehicle_id, distance, cargo_weight, cost FROM orders";
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) return 0;
    
    printf("\n=== All orders ===\n");
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        printf("%d | %s | Driver: %d | Vehicle: %d | Distance: %d | Cargo: %.2f | Cost: %.2f\n",
            sqlite3_column_int(stmt, 0),
            sqlite3_column_text(stmt, 1),
            sqlite3_column_int(stmt, 2),
            sqlite3_column_int(stmt, 3),
            sqlite3_column_int(stmt, 4),
            sqlite3_column_double(stmt, 5),
            sqlite3_column_double(stmt, 6));
    }
    
    sqlite3_finalize(stmt);
    return 1;
}

int order_delete(sqlite3 *db, int id) {
    char sql[256];
    char *err_msg = 0;
    sprintf(sql, "DELETE FROM orders WHERE order_id = %d;", id);
    
    int rc = db_execute(db, sql, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Delete error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return 0;
    }
    return 1;
}
