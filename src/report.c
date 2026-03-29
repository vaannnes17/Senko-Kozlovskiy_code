#include "report.h"
#include "database.h"
#include <stdio.h>

int report_driver_orders(sqlite3 *db, int driver_id, const char *start_date, const char *end_date) {
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
    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        printf("Order ID: %d | Date: %s | Vehicle: %d | Distance: %d | Cargo: %.2f | Cost: %.2f\n",
            sqlite3_column_int(stmt, 0),
            sqlite3_column_text(stmt, 1),
            sqlite3_column_int(stmt, 2),
            sqlite3_column_int(stmt, 3),
            sqlite3_column_double(stmt, 4),
            sqlite3_column_double(stmt, 5));
        count++;
    }
    
    if (count == 0) {
        printf("No orders found\n");
    }
    
    sqlite3_finalize(stmt);
    return 1;
}

int report_vehicle_stats(sqlite3 *db, int vehicle_id) {
    sqlite3_stmt *stmt;
    char sql[256];
    sprintf(sql, "SELECT total_mileage, total_cargo, total_trips FROM vehicle_stats WHERE vehicle_id = ?");
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) return 0;
    
    sqlite3_bind_int(stmt, 1, vehicle_id);
    rc = sqlite3_step(stmt);
    
    if (rc == SQLITE_ROW) {
        int mileage = sqlite3_column_int(stmt, 0);
        double cargo = sqlite3_column_double(stmt, 1);
        int trips = sqlite3_column_int(stmt, 2);
        
        printf("\n=== Vehicle ID %d statistics ===\n", vehicle_id);
        printf("Total mileage: %d km\n", mileage);
        printf("Total cargo: %.2f tons\n", cargo);
        printf("Total trips: %d\n", trips);
    } else {
        printf("No statistics found for vehicle ID %d\n", vehicle_id);
    }
    
    sqlite3_finalize(stmt);
    return 1;
}

int report_all_drivers_stats(sqlite3 *db) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT d.driver_id, d.last_name, d.first_name, "
                      "ds.total_trips, ds.total_cargo, ds.total_earnings "
                      "FROM drivers d JOIN driver_stats ds ON d.driver_id = ds.driver_id";
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) return 0;
    
    printf("\n=== All drivers statistics ===\n");
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        printf("ID: %d | %s %s | Trips: %d | Cargo: %.2f | Earnings: %.2f\n",
            sqlite3_column_int(stmt, 0),
            sqlite3_column_text(stmt, 1),
            sqlite3_column_text(stmt, 2),
            sqlite3_column_int(stmt, 3),
            sqlite3_column_double(stmt, 4),
            sqlite3_column_double(stmt, 5));
    }
    
    sqlite3_finalize(stmt);
    return 1;
}

int report_worst_driver(sqlite3 *db) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT d.driver_id, d.last_name, d.first_name, d.patronymic, "
                      "d.category, d.experience, d.address, d.birth_year, "
                      "ds.total_trips, ds.total_earnings "
                      "FROM drivers d JOIN driver_stats ds ON d.driver_id = ds.driver_id "
                      "ORDER BY ds.total_trips ASC LIMIT 1";
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) return 0;
    
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        printf("\n=== Driver with fewest trips ===\n");
        printf("ID: %d\n", sqlite3_column_int(stmt, 0));
        printf("Name: %s %s %s\n", 
            sqlite3_column_text(stmt, 1),
            sqlite3_column_text(stmt, 2),
            sqlite3_column_text(stmt, 3) ? (const char*)sqlite3_column_text(stmt, 3) : "");
        printf("Category: %s\n", sqlite3_column_text(stmt, 4));
        printf("Experience: %d years\n", sqlite3_column_int(stmt, 5));
        printf("Address: %s\n", sqlite3_column_text(stmt, 6));
        printf("Birth year: %d\n", sqlite3_column_int(stmt, 7));
        printf("Total trips: %d\n", sqlite3_column_int(stmt, 8));
        printf("Total earnings: %.2f\n", sqlite3_column_double(stmt, 9));
    }
    
    sqlite3_finalize(stmt);
    return 1;
}

int report_best_vehicle(sqlite3 *db) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT v.vehicle_id, v.plate_number, v.brand, v.mileage_on_purchase, v.capacity, "
                      "vs.total_mileage, vs.total_cargo, vs.total_trips "
                      "FROM vehicles v JOIN vehicle_stats vs ON v.vehicle_id = vs.vehicle_id "
                      "ORDER BY vs.total_mileage DESC LIMIT 1";
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) return 0;
    
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        printf("\n=== Vehicle with highest total mileage ===\n");
        printf("ID: %d\n", sqlite3_column_int(stmt, 0));
        printf("Plate number: %s\n", sqlite3_column_text(stmt, 1));
        printf("Brand: %s\n", sqlite3_column_text(stmt, 2));
        printf("Mileage on purchase: %d km\n", sqlite3_column_int(stmt, 3));
        printf("Capacity: %.2f tons\n", sqlite3_column_double(stmt, 4));
        printf("Total mileage: %d km\n", sqlite3_column_int(stmt, 5));
        printf("Total cargo: %.2f tons\n", sqlite3_column_double(stmt, 6));
        printf("Total trips: %d\n", sqlite3_column_int(stmt, 7));
    }
    
    sqlite3_finalize(stmt);
    return 1;
}
