cmake_minimum_required(VERSION 3.15.0)

project(ModbusBasic)

# Enable testing
enable_testing()

# Set up GoogleTest as a subdirectory. It assumes you have googletest located in external/googletest.
# This will build GoogleTest inside the build directory as well.
add_subdirectory(external/googletest)

# Add the tests subdirectory.
# This assumes that you have a 'tests' directory in your project, where your test code resides.
add_subdirectory(tests)

# Optional: Set CMake build type (Release, Debug, etc.)
set(CMAKE_BUILD_TYPE Release)  # You can also set it to Debug, RelWithDebInfo, etc.

# Optional: Define output directories for binaries and libraries (useful for keeping everything in the build folder)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
