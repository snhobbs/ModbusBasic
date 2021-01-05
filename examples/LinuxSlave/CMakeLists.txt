CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(CMAKE_CXX_COMPILER g++-8)
add_compile_options(-fsanitize=address, -fno-omit-frame-pointer)
add_compile_options(-fsanitize=undefined)

project(modbus_client)

SET(CMAKE_VERBOSE_MAKEFILE ON)

ADD_EXECUTABLE(${PROJECT_NAME} source/main.cpp)
target_link_libraries( ${PROJECT_NAME} asan)
#target_link_libraries( ${PROJECT_NAME} tsan)
target_link_libraries( ${PROJECT_NAME} ubsan)
#target_link_libraries( ${PROJECT_NAME} msan)

#find_package(Modbus 1.0.1 REQUIRED)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../common)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/source)
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIR}/DataStores/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIR}/CppUtilities/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../posix)

target_compile_options(
  ${PROJECT_NAME}
  PUBLIC
  -Wall
  -Wextra
  -Wpedantic
  -Wfatal-errors
)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
