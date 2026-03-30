add_library(autopark_lib STATIC
    database.c
    driver.c
    vehicle.c
    order.c
    report.c
    utils.c
)

target_include_directories(autopark_lib PUBLIC ${CMAKE_SOURCE_DIR}/include)
