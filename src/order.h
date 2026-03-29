#ifndef ORDER_H
#define ORDER_H

#include <sqlite3.h>

typedef struct {
    int id;
    char order_date[11];
    int driver_id;
    int vehicle_id;
    int distance;
    double cargo_weight;
    double cost;
} Order;

int order_create_table(sqlite3 *db);
int order_insert(sqlite3 *db, const Order *o);
int order_select_by_driver(sqlite3 *db, int driver_id, const char *start_date, const char *end_date);
int order_select_by_vehicle(sqlite3 *db, int vehicle_id);
int order_select_all(sqlite3 *db);
int order_delete(sqlite3 *db, int id);
int order_check_capacity(sqlite3 *db, int vehicle_id, double cargo_weight);

#endif
