#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include "../src/database.c"
#include "../src/driver.c"
#include "../src/vehicle.c"
#include "../src/order.c"

int main() {
    sqlite3 *db;
    Driver d;
    char *err_msg = 0;
    
    sqlite3_open(":memory:", &db);
    
    driver_create_table(db);
    vehicle_create_table(db);
    order_create_table(db);
    
    d.id = 0;
    strcpy(d.personnel_number, "001");
    strcpy(d.last_name, "Testov");
    strcpy(d.first_name, "Test");
    strcpy(d.patronymic, "Testovich");
    strcpy(d.category, "B");
    d.experience = 10;
    strcpy(d.address, "Test St, 1");
    d.birth_year = 1980;
    
    if (driver_insert(db, &d)) {
        printf("TEST 1 PASSED: Insert driver\n");
    } else {
        printf("TEST 1 FAILED: Insert driver\n");
    }
    
    Driver found;
    if (driver_select_by_id(db, 1, &found)) {
        if (strcmp(found.last_name, "Testov") == 0) {
            printf("TEST 2 PASSED: Select driver by ID\n");
        } else {
            printf("TEST 2 FAILED: Select driver by ID - wrong name\n");
        }
    } else {
        printf("TEST 2 FAILED: Driver not found\n");
    }
    
    if (driver_update(db, 1, 15)) {
        driver_select_by_id(db, 1, &found);
        if (found.experience == 15) {
            printf("TEST 3 PASSED: Update driver experience\n");
        } else {
            printf("TEST 3 FAILED: Update driver experience - wrong value\n");
        }
    } else {
        printf("TEST 3 FAILED: Update failed\n");
    }
    
    if (driver_delete(db, 1)) {
        if (!driver_select_by_id(db, 1, &found)) {
            printf("TEST 4 PASSED: Delete driver\n");
        } else {
            printf("TEST 4 FAILED: Driver still exists\n");
        }
    } else {
        printf("TEST 4 FAILED: Delete failed\n");
    }
    
    sqlite3_close(db);
    
    printf("\nAll tests completed!\n");
    return 0;
}
