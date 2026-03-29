#ifndef DRIVER_H
#define DRIVER_H

#include <sqlite3.h>

typedef struct {
    int id;
    char personnel_number[50];
    char last_name[100];
    char first_name[100];
    char patronymic[100];
    char category[10];
    int experience;
    char address[200];
    int birth_year;
} Driver;

int driver_create_table(sqlite3 *db);
int driver_insert(sqlite3 *db, const Driver *d);
int driver_select_by_id(sqlite3 *db, int id, Driver *d);
int driver_select_by_lastname(sqlite3 *db, const char *lastname);
int driver_select_all(sqlite3 *db);
int driver_update(sqlite3 *db, int id, int new_experience);
int driver_delete(sqlite3 *db, int id);

#endif

