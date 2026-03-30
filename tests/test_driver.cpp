#include <gtest/gtest.h>
#include <sqlite3.h>
#include <string.h>

// Подключаем C-функции
extern "C" {
    #include "../include/config.h"
    #include "../src/database.h"
    #include "../src/driver.h"
    #include "../src/vehicle.h"
    #include "../src/order.h"
}

class DatabaseTest : public ::testing::Test {
protected:
    sqlite3 *db;
    
    void SetUp() override {
        sqlite3_open(":memory:", &db);
        driver_create_table(db);
        vehicle_create_table(db);
        order_create_table(db);
    }
    
    void TearDown() override {
        sqlite3_close(db);
    }
};

TEST_F(DatabaseTest, InsertDriver) {
    Driver d;
    d.id = 0;
    strcpy(d.personnel_number, "001");
    strcpy(d.last_name, "Testov");
    strcpy(d.first_name, "Test");
    strcpy(d.patronymic, "Testovich");
    strcpy(d.category, "B");
    d.experience = 10;
    strcpy(d.address, "Test St, 1");
    d.birth_year = 1980;
    
    int result = driver_insert(db, &d);
    EXPECT_EQ(result, 1);
}

TEST_F(DatabaseTest, SelectDriverById) {
    Driver d;
    d.id = 0;
    strcpy(d.personnel_number, "001");
    strcpy(d.last_name, "Testov");
    strcpy(d.first_name, "Test");
    strcpy(d.patronymic, "Testovich");
    strcpy(d.category, "B");
    d.experience = 10;
    strcpy(d.address, "Test St, 1");
    d.birth_year = 1980;
    
    driver_insert(db, &d);
    
    Driver found;
    int result = driver_select_by_id(db, 1, &found);
    
    EXPECT_EQ(result, 1);
    EXPECT_STREQ(found.last_name, "Testov");
    EXPECT_EQ(found.experience, 10);
}

TEST_F(DatabaseTest, UpdateDriver) {
    Driver d;
    d.id = 0;
    strcpy(d.personnel_number, "001");
    strcpy(d.last_name, "Testov");
    strcpy(d.first_name, "Test");
    strcpy(d.patronymic, "Testovich");
    strcpy(d.category, "B");
    d.experience = 10;
    strcpy(d.address, "Test St, 1");
    d.birth_year = 1980;
    
    driver_insert(db, &d);
    
    int result = driver_update(db, 1, 15);
    EXPECT_EQ(result, 1);
    
    Driver found;
    driver_select_by_id(db, 1, &found);
    EXPECT_EQ(found.experience, 15);
}

TEST_F(DatabaseTest, DeleteDriver) {
    Driver d;
    d.id = 0;
    strcpy(d.personnel_number, "001");
    strcpy(d.last_name, "Testov");
    strcpy(d.first_name, "Test");
    strcpy(d.patronymic, "Testovich");
    strcpy(d.category, "B");
    d.experience = 10;
    strcpy(d.address, "Test St, 1");
    d.birth_year = 1980;
    
    driver_insert(db, &d);
    
    int result = driver_delete(db, 1);
    EXPECT_EQ(result, 1);
    
    Driver found;
    result = driver_select_by_id(db, 1, &found);
    EXPECT_EQ(result, 0);
}

TEST_F(DatabaseTest, InsertVehicle) {
    Vehicle v;
    v.id = 0;
    strcpy(v.plate_number, "A123BE");
    strcpy(v.brand, "KAMAZ");
    v.mileage_on_purchase = 50000;
    v.capacity = 10.0;
    
    int result = vehicle_insert(db, &v);
    EXPECT_EQ(result, 1);
}

TEST_F(DatabaseTest, CheckCapacity) {
    Vehicle v;
    v.id = 0;
    strcpy(v.plate_number, "A123BE");
    strcpy(v.brand, "KAMAZ");
    v.mileage_on_purchase = 50000;
    v.capacity = 10.0;
    
    vehicle_insert(db, &v);
    
    int result = order_check_capacity(db, 1, 8.0);
    EXPECT_EQ(result, 1);
    
    result = order_check_capacity(db, 1, 12.0);
    EXPECT_EQ(result, 0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
