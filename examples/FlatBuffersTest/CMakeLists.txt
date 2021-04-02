CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(CMAKE_CXX_COMPILER g++-8)
add_compile_options(-fsanitize=address, -fno-omit-frame-pointer)
add_compile_options(-fsanitize=undefined)

set (TargetName modbus_client)
project(modbus_client)

SET(CMAKE_VERBOSE_MAKEFILE ON)

ADD_EXECUTABLE(${PROJECT_NAME} main.cpp)
target_link_libraries( ${PROJECT_NAME} asan)
#target_link_libraries( ${PROJECT_NAME} tsan)
target_link_libraries( ${PROJECT_NAME} ubsan)
#target_link_libraries( ${PROJECT_NAME} msan)

#find_package(Modbus 1.0.1 REQUIRED)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../common)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIR}/DataStores/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIR}/CppUtilities/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIR}/HardwareInterfaces/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIR}/ModbusBasic/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIR}/Modbus)
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIR}/../../flatbuffers/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../posix)

target_compile_options(
  ${PROJECT_NAME}
  PUBLIC
  -Wall
  -Wextra
  -Wpedantic
  -Wfatal-errors
)
  set(source_files "InputRegisterMappedDataStore_generated.h;HoldingRegisterMappedDataStore_generated.h")
  list(TRANSFORM source_files PREPEND ${directory}/)

  set(script "${ScriptsDirectory}/RegistersMapping.py")

  target_sources(${TargetName} PUBLIC ${source_files})
  set_source_files_properties(${source_files} GENERATED)

  add_custom_target(
    flatbuffers
    )

  set(name "HoldingRegisterMappedDataStore")
  add_custom_command(
    TARGET flatbuffers
    COMMENT "Generating Modbus Source ${source_files}"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS "${name}.fbs"
    BYPRODUCTS "${name}_generated.h"
    COMMAND /home/simon/software/flatbuffers/flatc --cpp --grpc --cpp-std=c++17 --cpp-ptr-type naked --gen-mutable --scoped-enums --gen-name-strings --cpp-static-reflection --gen-object-api
    ARGS  ${name}.fbs
    )
  set(name "InputRegisterMappedDataStore")
  add_custom_command(
    TARGET flatbuffers
    COMMENT "Generating Modbus Source ${source_files}"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS "${name}.fbs"
    BYPRODUCTS "${name}_generated.h"
    COMMAND /home/simon/software/flatbuffers/flatc --cpp --grpc --cpp-std=c++17 --cpp-ptr-type naked --gen-mutable --scoped-enums --gen-name-strings --cpp-static-reflection --gen-object-api
    ARGS  ${name}.fbs
    )


set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
