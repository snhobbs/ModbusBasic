cmake_minimum_required(VERSION 3.15.0)

set(TargetName modbus_basic_tests)
project(${TargetName})

add_executable(${TargetName})

function(Generate name)
  set(source_name "${name}_generated.h")
  set_source_files_properties(${source_name} GENERATED)
  target_sources(${TargetName} PUBLIC ${source_name})
  add_custom_command(
    COMMENT "Generating source ${name}"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS "${name}.fbs"
    OUTPUT "${name}_generated.h"
    COMMAND flatc --cpp --grpc --cpp-std=c++17 --cpp-ptr-type naked --gen-mutable --scoped-enums --gen-name-strings --cpp-static-reflection --gen-object-api
    ARGS  ${name}.fbs
    )
endfunction()

Generate("input_registers")
Generate("holding_registers")


target_include_directories(${TargetName} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../include")

#SET(CMAKE_CXX_COMPILER clang++-9)
#SET(CMAKE_CXX_COMPILER /usr/bin/g++-8)
option(RUN_SANITIZER ON)

if(${RUN_SANITIZER})
target_compile_options(
${TargetName}
PRIVATE
    -fsanitize=address
    -fno-omit-frame-pointer
    -fsanitize=undefined)

target_link_libraries( ${TargetName} asan)
#target_link_libraries( ${TargetName} tsan)
target_link_libraries( ${TargetName} ubsan)
#target_link_libraries( ${TargetName} msan)
endif()

target_compile_options(
${TargetName}
PRIVATE
    -Wall
    -Wextra
    -pedantic
    -Wpedantic
    #-pedantic-errors
    #-Werror=pedantic

    -Wconversion
    $<$<COMPILE_LANGUAGE:C>:-Wimplicit>
    $<$<COMPILE_LANGUAGE:C>:-Wunused>

    #$<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>
    #$<$<COMPILE_LANGUAGE:CXX>:-Wabi>
    $<$<CXX_COMPILER_ID:GNU>:-Wsuggest-final-types>
    $<$<CXX_COMPILER_ID:GNU>:-Wmaybe-uninitialized>
    $<$<CXX_COMPILER_ID:GNU>:-Wdouble-promotion>
    $<$<CXX_COMPILER_ID:GNU>:-Wnull-dereference>
    -Winit-self

    $<$<CXX_COMPILER_ID:GNU>:-Wduplicated-cond>
    $<$<CXX_COMPILER_ID:GNU>:-Wduplicated-branches>
    $<$<CXX_COMPILER_ID:GNU>:-Wlogical-op>
    $<$<CXX_COMPILER_ID:GNU>:-Wrestrict>
    $<$<C_COMPILER_ID:GNU>:$<$<COMPILE_LANGUAGE:C>:-Wjump-misses-init>>
    $<$<COMPILE_LANGUAGE:CXX>:-Wold-style-cast>
    -Wshadow
)

target_compile_features(${TargetName} PUBLIC cxx_std_17)
target_compile_definitions(${TargetName} PRIVATE TESTING)
target_compile_definitions(${TargetName} PRIVATE UNITTESTING)
target_compile_definitions(${TargetName} PRIVATE LINUX)
set_property(TARGET ${TargetName} PROPERTY CXX_STANDARD 17)
#add_definitions(-D__CORE_CMINSTR_H)
#add_definitions(-D__CORE_CMINSTR_H)
#add_definitions(-D__GNUC__)

#target_compile_options(${TargetName} PRIVATE -fpermissive)
#target_compile_options(-fprofile-arcs -ftest-coverage)

#SET(GCC_COVERAGE_LINK_FLAGS    "-coverage -lgcov")
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")

#set(FSL_FEATURE_ADC_HAS_CTRL_LPWRMODE true)
SET(CMAKE_VERBOSE_MAKEFILE ON)

#fixme this should have all the sources
set(TestSource ${CMAKE_CURRENT_SOURCE_DIR}/source)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/source sources)

target_sources(${TargetName} PUBLIC ${sources})
target_sources(${TargetName} PUBLIC ${TestSource}/main.cpp)


#find_library(GTEST_LIB NAMES gtest)
find_library(GTEST_LIB gtest REQUIRED)
#set(GTEST_LIB  "/usr/src/googletest/lib/libgtest.a")
message("GTEST: ${GTEST_LIB}")

find_library(PTHREAD_LIB pthread)
message("PTHREAD: ${PTHREAD_LIB}")
target_link_libraries(${TargetName} ${GTEST_LIB})
#target_link_libraries(${TargetName} gtest)
target_link_libraries(${TargetName} ${PTHREAD_LIB})

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../include)
include_directories(${TargetName} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${TargetName} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/source)
include_directories(${TargetName} PUBLIC ${INCLUDE_DIR})
target_include_directories(${TargetName} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/source)
include_directories(${TargetName} PUBLIC ${INCLUDE_DIR}/CppUtilities/include)
target_include_directories(${TargetName} PUBLIC ${INCLUDE_DIR}/CppUtilities/include/sprout)
target_include_directories(${TargetName} PUBLIC ${INCLUDE_DIR}/DataStores/include)
target_include_directories(${TargetName} PUBLIC /home/simon/software/flatbuffers/include)

if(RUN_ANALYSIS)
  include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeStaticAnalysis/StaticAnalysis.cmake)
  include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeStaticAnalysis/Utilities.cmake)
GetStaticAnalysisFiles(${TargetName} "${StaticExcludes}")
message(STATUS "Analyze ${AnalyseFiles}")
StaticAnalysis("${AnalyseFiles}")
endif()

add_test(NAME ${TargetName} COMMAND ${TargetName})
