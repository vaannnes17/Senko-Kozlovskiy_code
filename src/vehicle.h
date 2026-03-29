#ifndef VEHICLE_H
#define VEHICLE_H

#include <sqlite3.h>

typedef struct {
    int id;
    char plate_number[20];
    char brand[50];
    int mileage_on_purchase;
    double capacity;
} Vehicle;

int vehicle_create_table(sqlite3 *db);
int vehicle_insert(sqlite3 *db, const Vehicle *v);
int vehicle_select_by_id(sqlite3 *db, int id, Vehicle *v);
int vehicle_select_all(sqlite3 *db);
int vehicle_update(sqlite3 *db, int id, int new_mileage);
int vehicle_delete(sqlite3 *db, int id);
double vehicle_get_capacity(sqlite3 *db, int id);

#endif
