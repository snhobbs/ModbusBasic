# - Config file for the Modbus package
# It defines the following variables
#  MODBUS_INCLUDE_DIRS - include directories for Modbus
#  MODBUS_LIBRARIES    - libraries to link against
#  MODBUS_EXECUTABLE   - the bar executable

# Compute paths
get_filename_component(MODBUS_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
set(MODBUS_INCLUDE_DIRS ${MODBUS_CMAKE_DIR}/include)
