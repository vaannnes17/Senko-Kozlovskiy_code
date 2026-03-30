include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.tar.gz
)
FetchContent_MakeAvailable(googletest)

add_executable(run_tests test_driver.cpp)

target_link_libraries(run_tests
    autopark_lib
    sqlite3
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(run_tests)
