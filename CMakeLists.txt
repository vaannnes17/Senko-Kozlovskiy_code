cmake_minimum_required(VERSION 3.14)
project(Autopark)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)

# Добавляем поддиректории
add_subdirectory(src)
add_subdirectory(tests)
