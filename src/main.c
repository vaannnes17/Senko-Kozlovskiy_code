#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "database.h"
#include "driver.h"
#include "vehicle.h"
#include "order.h"
#include "report.h"
#include "utils.h"
#include "config.h"

void print_menu() {
    printf("\n========================================\n");
    printf("   Autopark Management System\n");
    printf("========================================\n");
    printf("1. Create all tables\n");
    printf("2. Add driver\n");
    printf("3. Add vehicle\n");
    printf("4. Add order\n");
    printf("5. View all drivers\n");
    printf("6. View all vehicles\n");
    printf("7. View all orders\n");
    printf("8. Find driver by last name\n");
    printf("9. Update driver experience\n");
    printf("10. Delete driver\n");
    printf("11. Delete vehicle\n");
    printf("12. Delete order\n");
    printf("13. Report: driver orders by period\n");
    printf("14. Report: vehicle statistics\n");
    printf("15. Report: all drivers statistics\n");
    printf("16. Report: driver with fewest trips\n");
    printf("17. Report: vehicle with highest mileage\n");
    printf("0. Exit\n");
    printf("========================================\n");
    printf("Choose option: ");
}

int main() {
    sqlite3 *db;
    Driver d;
    Vehicle v;
    Order o;
    char input[256];
    int choice, id, driver_id, vehicle_id, experience, distance;
    double cargo_weight, cost, capacity;
    char lastname[100], start_date[11], end_date[11], date[11];
    
    if (!db_open(&db)) {
        return 1;
    }
    printf("Database opened successfully\n");
    
    while (1) {
        print_menu();
        fgets(input, sizeof(input), stdin);
        choice = atoi(input);
        
        switch (choice) {
            case 1:
                driver_create_table(db);
                vehicle_create_table(db);
                order_create_table(db);
                break;
                
            case 2:
                printf("\n=== Add driver ===\n");
                printf("Personnel number: "); fgets(d.personnel_number, sizeof(d.personnel_number), stdin); trim(d.personnel_number);
                printf("Last name: "); fgets(d.last_name, sizeof(d.last_name), stdin); trim(d.last_name);
                printf("First name: "); fgets(d.first_name, sizeof(d.first_name), stdin); trim(d.first_name);
                printf("Patronymic: "); fgets(d.patronymic, sizeof(d.patronymic), stdin); trim(d.patronymic);
                printf("Category (B/C/D/E): "); fgets(d.category, sizeof(d.category), stdin); trim(d.category);
                printf("Experience (years): "); fgets(input, sizeof(input), stdin); d.experience = atoi(input);
                printf("Address: "); fgets(d.address, sizeof(d.address), stdin); trim(d.address);
                printf("Birth year: "); fgets(input, sizeof(input), stdin); d.birth_year = atoi(input);
                
                if (driver_insert(db, &d)) {
                    printf("Driver added successfully\n");
                } else {
                    printf("Failed to add driver\n");
                }
                break;
                
            case 3:
                printf("\n=== Add vehicle ===\n");
                printf("Plate number: "); fgets(v.plate_number, sizeof(v.plate_number), stdin); trim(v.plate_number);
                printf("Brand: "); fgets(v.brand, sizeof(v.brand), stdin); trim(v.brand);
                printf("Mileage on purchase (km): "); fgets(input, sizeof(input), stdin); v.mileage_on_purchase = atoi(input);
                printf("Capacity (tons): "); fgets(input, sizeof(input), stdin); v.capacity = atof(input);
                
                if (vehicle_insert(db, &v)) {
                    printf("Vehicle added successfully\n");
                } else {
                    printf("Failed to add vehicle\n");
                }
                break;
                
            case 4:
                printf("\n=== Add order ===\n");
                printf("Date (YYYY-MM-DD): "); fgets(o.order_date, sizeof(o.order_date), stdin); trim(o.order_date);
                printf("Driver ID: "); fgets(input, sizeof(input), stdin); o.driver_id = atoi(input);
                printf("Vehicle ID: "); fgets(input, sizeof(input), stdin); o.vehicle_id = atoi(input);
                printf("Distance (km): "); fgets(input, sizeof(input), stdin); o.distance = atoi(input);
                printf("Cargo weight (tons): "); fgets(input, sizeof(input), stdin); o.cargo_weight = atof(input);
                printf("Cost: "); fgets(input, sizeof(input), stdin); o.cost = atof(input);
                
                capacity = vehicle_get_capacity(db, o.vehicle_id);
                if (capacity < 0) {
                    printf("Vehicle not found\n");
                } else if (o.cargo_weight > capacity) {
                    printf("Error: cargo weight (%.2f) exceeds vehicle capacity (%.2f)\n", o.cargo_weight, capacity);
                } else if (order_insert(db, &o)) {
                    printf("Order added successfully (20%% earned added to driver)\n");
                } else {
                    printf("Failed to add order\n");
                }
                break;
                
            case 5:
                driver_select_all(db);
                break;
                
            case 6:
                vehicle_select_all(db);
                break;
                
            case 7:
                order_select_all(db);
                break;
                
            case 8:
                printf("Enter last name (or start of last name): ");
                fgets(lastname, sizeof(lastname), stdin);
                trim(lastname);
                driver_select_by_lastname(db, lastname);
                break;
                
            case 9:
                printf("Enter driver ID: ");
                fgets(input, sizeof(input), stdin);
                id = atoi(input);
                printf("Enter new experience (years): ");
                fgets(input, sizeof(input), stdin);
                experience = atoi(input);
                if (driver_update(db, id, experience)) {
                    printf("Experience updated\n");
                } else {
                    printf("Update failed\n");
                }
                break;
                
            case 10:
                printf("Enter driver ID to delete: ");
                fgets(input, sizeof(input), stdin);
                id = atoi(input);
                if (driver_delete(db, id)) {
                    printf("Driver deleted\n");
                } else {
                    printf("Delete failed\n");
                }
                break;
                
            case 11:
                printf("Enter vehicle ID to delete: ");
                fgets(input, sizeof(input), stdin);
                id = atoi(input);
                if (vehicle_delete(db, id)) {
                    printf("Vehicle deleted\n");
                } else {
                    printf("Delete failed\n");
                }
                break;
                
            case 12:
                printf("Enter order ID to delete: ");
                fgets(input, sizeof(input), stdin);
                id = atoi(input);
                if (order_delete(db, id)) {
                    printf("Order deleted\n");
                } else {
                    printf("Delete failed\n");
                }
                break;
                
            case 13:
                printf("Enter driver ID: ");
                fgets(input, sizeof(input), stdin);
                driver_id = atoi(input);
                printf("Start date (YYYY-MM-DD): ");
                fgets(start_date, sizeof(start_date), stdin);
                trim(start_date);
                printf("End date (YYYY-MM-DD): ");
                fgets(end_date, sizeof(end_date), stdin);
                trim(end_date);
                report_driver_orders(db, driver_id, start_date, end_date);
                break;
                
            case 14:
                printf("Enter vehicle ID: ");
                fgets(input, sizeof(input), stdin);
                vehicle_id = atoi(input);
                report_vehicle_stats(db, vehicle_id);
                break;
                
            case 15:
                report_all_drivers_stats(db);
                break;
                
            case 16:
                report_worst_driver(db);
                break;
                
            case 17:
                report_best_vehicle(db);
                break;
                
            case 0:
                db_close(db);
                printf("Goodbye!\n");
                return 0;
                
            default:
                printf("Invalid option\n");
        }
    }
}
