cmake_minimum_required(VERSION 3.15.0)

project(tests)
add_executable(tests)

target_include_directories(tests PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../include")

#SET(CMAKE_CXX_COMPILER clang++-9)
SET(CMAKE_CXX_COMPILER /usr/bin/g++-8)
option(RUN_SANITIZER ON)

if(${RUN_SANITIZER})
target_compile_options(
tests
PRIVATE
    -fsanitize=address
    -fno-omit-frame-pointer
    -fsanitize=undefined)

target_link_libraries( tests asan)
#target_link_libraries( tests tsan)
target_link_libraries( tests ubsan)
#target_link_libraries( tests msan)
endif()

target_compile_options(
tests
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

target_compile_features(tests PUBLIC cxx_std_17)
target_compile_definitions(tests PRIVATE TESTING)
target_compile_definitions(tests PRIVATE UNITTESTING)
target_compile_definitions(tests PRIVATE LINUX)
set_property(TARGET tests PROPERTY CXX_STANDARD 17)
#add_definitions(-D__CORE_CMINSTR_H)
#add_definitions(-D__CORE_CMINSTR_H)
#add_definitions(-D__GNUC__)

#target_compile_options(tests PRIVATE -fpermissive)
#target_compile_options(-fprofile-arcs -ftest-coverage)

#SET(GCC_COVERAGE_LINK_FLAGS    "-coverage -lgcov")
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")

#set(FSL_FEATURE_ADC_HAS_CTRL_LPWRMODE true)
SET(CMAKE_VERBOSE_MAKEFILE ON)

#fixme this should have all the sources
set(TestSource ${CMAKE_CURRENT_SOURCE_DIR}/source)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/source sources)

target_sources(tests PUBLIC ${sources})
target_sources(tests PUBLIC ${TestSource}/main.cpp)


#find_library(GTEST_LIB NAMES gtest)
find_library(GTEST_LIB gtest REQUIRED)
#set(GTEST_LIB  "/usr/src/googletest/lib/libgtest.a")
message("GTEST: ${GTEST_LIB}")

find_library(PTHREAD_LIB pthread)
message("PTHREAD: ${PTHREAD_LIB}")
target_link_libraries(tests ${GTEST_LIB})
#target_link_libraries(tests gtest)
target_link_libraries(tests ${PTHREAD_LIB})

include_directories(tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/source)
include_directories(tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_include_directories(tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/source)
target_include_directories(tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)

if(RUN_ANALYSIS)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/StaticAnalysis.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Utilities.cmake)
GetStaticAnalysisFiles(tests "${StaticExcludes}")
message(STATUS "Analyze ${AnalyseFiles}")
StaticAnalysis("${AnalyseFiles}")
endif()

add_test(NAME tests COMMAND tests)
