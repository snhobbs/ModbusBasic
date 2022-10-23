CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

set(CMAKE_CXX_COMPILER /usr/bin/g++-8)


project(libmodbus_slave)
add_compile_options(-Wall -Wextra -Wpedantic -Wfatal-errors)
add_compile_options(-std=c++17)
add_compile_options(-fpermissive)

SET(CMAKE_VERBOSE_MAKEFILE ON)

ADD_EXECUTABLE(${PROJECT_NAME} libmodbus_slave.cpp)

target_link_libraries( ${PROJECT_NAME} modbus)

target_include_directories(${PROJECT_NAME} PRIVATE /usr/include/modbus )
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/..)
