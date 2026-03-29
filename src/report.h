#ifndef REPORT_H
#define REPORT_H

#include <sqlite3.h>

int report_driver_orders(sqlite3 *db, int driver_id, const char *start_date, const char *end_date);
int report_vehicle_stats(sqlite3 *db, int vehicle_id);
int report_all_drivers_stats(sqlite3 *db);
int report_worst_driver(sqlite3 *db);
int report_best_vehicle(sqlite3 *db);

#endif
