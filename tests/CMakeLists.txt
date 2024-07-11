cmake_minimum_required(VERSION 3.10.0)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
project(Testing)
set(ProjectDirectory "${CMAKE_CURRENT_SOURCE_DIR}/..")
set(LibraryDirectory ${ProjectDirectory}/external)
#SET(CMAKE_CXX_COMPILER clang++-9)
SET(CMAKE_CXX_COMPILER /usr/bin/g++-9)
else()
set(ProjectDirectory "${CMAKE_SOURCE_DIR}")
endif()

message("\n\nProject directory ${ProjectDirectory}\n\n")

set(TargetName "testing")
add_executable(${TargetName})
set(ProjectSource ${ProjectDirectory}/src)
set(BoardDirectory ${ProjectDirectory}/board)
set(SourceDirectory ${CMAKE_CURRENT_SOURCE_DIR}/source)
set(TestSources ${SourceDirectory})
message("proj source ${ProjectSource}")

target_include_directories(${TargetName} PUBLIC "${SourceDirectory}")

message("\n\nTest include directory ${LibraryDirectory}\n\n")
set(gtest_DIR "${CMAKE_CURRENT_SOURCE_DIR}/googletest/googletest")
set(gmock_DIR "${CMAKE_CURRENT_SOURCE_DIR}/googletest/googlemock")
target_include_directories(${TargetName} PUBLIC "${gtest_DIR}/include")
target_include_directories(${TargetName} PUBLIC "${ProjectDirectory}/lib/CppUtilities/include")
target_include_directories(${TargetName} PUBLIC "${ProjectDirectory}/include")


# Add Sources
set(DIR_SRCS
  #${TestSources}/test_Coils.cpp
  # ${TestSources}/test_ConnectedDevice.cpp
  ${TestSources}/test_Crc.cpp
  ${TestSources}/test_DataConversion.cpp
  # ${TestSources}/test_HoldingRegisters.cpp
  ${TestSources}/test_MappedHoldingRegisterController.cpp
  #${TestSources}/test_MappedInputRegisterController.cpp
  ${TestSources}/test_MappedRegisterDataStore.cpp
  # ${TestSources}/test_RtuMaster.cpp
  ${TestSources}/test_RtuProtocol.cpp
  # ${TestSources}/test_RtuSlave.cpp
  ${TestSources}/test_buffer.cpp
  ${TestSources}/test_ringbuffer.cpp
  ${TestSources}/test_Modbus.cpp
  #${TestSources}/test_BitController.cpp
  ${TestSources}/test_RegisterController.cpp
  ${TestSources}/test_Accessor.cpp
  ${TestSources}/main.cpp
)

target_sources(${TargetName} PUBLIC ${DIR_SRCS})


# Add Data directory
#target_compile_definitions(
#  ${TargetName}
#  PUBLIC
#  -D__DIR__="${CMAKE_CURRENT_SOURCE_DIR}/data"
#)

# Compile definitions
#target_compile_definitions(
#    ${TargetName}
#    PUBLIC
#    -DMODBUS
#)

if(FALSE)
target_compile_options(
${TargetName}
PRIVATE
    -fsanitize=address
    -fno-omit-frame-pointer
    -fsanitize=undefined)

#target_link_directories(${TargetName} PUBLIC /usr/lib/clang/8/lib/linux/)
#target_link_libraries( ${TargetName} clang_rt.ubsan_standalone-x86_64.a)
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
    -g3
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
    #$<$<CXX_COMPILER_ID:GNU>:-Wnull-dereference>
    -Winit-self

    #$<$<CXX_COMPILER_ID:GNU>:-Wduplicated-cond>
    #$<$<CXX_COMPILER_ID:GNU>:-Wduplicated-branches>
    #$<$<CXX_COMPILER_ID:GNU>:-Wlogical-op>
    #$<$<CXX_COMPILER_ID:GNU>:-Wrestrict>
    $<$<C_COMPILER_ID:GNU>:$<$<COMPILE_LANGUAGE:C>:-Wjump-misses-init>>
    $<$<COMPILE_LANGUAGE:CXX>:-Wold-style-cast>
    -Wshadow
)

target_compile_features(${TargetName} PUBLIC cxx_std_17)
target_compile_definitions(${TargetName} PRIVATE TESTING)
target_compile_definitions(${TargetName} PRIVATE UNITTESTING)
target_compile_definitions(${TargetName} PRIVATE LINUX)
target_compile_definitions(${TargetName} PRIVATE DEBUG)
set_property(TARGET ${TargetName} PROPERTY CXX_STANDARD 17)
#add_definitions(-D__CORE_CMINSTR_H)
#add_definitions(-D__CORE_CMINSTR_H)
#add_definitions(-D__GNUC__)

#target_compile_options(${TargetName} PRIVATE -fpermissive)
#target_compile_options(-fprofile-arcs -ftest-coverage)

#SET(GCC_COVERAGE_LINK_FLAGS    "-coverage -lgcov")
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")

SET(CMAKE_VERBOSE_MAKEFILE ON)


#----------------------------------------------------------------------
# GTEST
#----------------------------------------------------------------------
target_include_directories(${TargetName} PUBLIC "${gtest_DIR}/include")
target_include_directories(${TargetName} PUBLIC "${gmock_DIR}/include")
target_link_directories(${TargetName} PUBLIC ${gtest_DIR}/../lib)
target_link_libraries(${TargetName} gtest)
target_link_libraries(${TargetName} gmock)
target_link_libraries(${TargetName} pthread)
#target_include_directories(${TargetName} PUBLIC "/usr/src/googletest/googletest/include")
#----------------------------------------------------------------------

#list(APPEND CMAKE_SYSTEM_INCLUDE_PATH "/usr/include")
target_include_directories(${TargetName} PUBLIC ${ProjectSource})
target_include_directories(${TargetName} PUBLIC ${ProjectSource}/../libs)


#----------------------------------------------------------------------
# Boost
#----------------------------------------------------------------------
#set(BOOST_ROOT /usr/lib/boost)
set(BOOST_INCLUDEDIR /usr/include)
set(BOOST_LIBRARYDIR /usr/lib/boost/stage/lib)
find_package(Boost COMPONENTS algorithm core)
target_include_directories(${TargetName} PUBLIC ${boost_DIR})

target_include_directories(${TargetName} PUBLIC "/usr/include")
include_directories(SYSTEM ~/.local/include/include)
include_directories(SYSTEM /usr/include)

message(STATUS "Sources: ${DIR_SRCS}")
message("System Path" ${CMAKE_SYSTEM_INCLUDE_PATH})
#------------------------------------------------------------
# Static Analysis
#------------------------------------------------------------
#if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
#  set(CMakeStaticAnalysis_DIR ${CMAKE_CURRENT_SOURCE_DIR}/CMakeStaticAnalysis)
#  include(${CMakeStaticAnalysis_DIR}/Utilities.cmake)
#  GetStaticAnalysisFiles("${TargetName}" "")
#  include(${CMakeStaticAnalysis_DIR}/StaticAnalysis.cmake)
#  StaticAnalysis("${AnalyseFiles}" "./")
#endif()

add_test(NAME tests COMMAND ${TargetName})
