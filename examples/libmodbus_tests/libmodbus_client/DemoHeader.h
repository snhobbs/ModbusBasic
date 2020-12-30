#pragma once

#include <modbus.h>
#include <string>
#include <cstdio>
#include <cerrno>
#include <unistd.h>

inline int ConnectRtu(modbus_t** ctx, const char* dev_name, const std::size_t baudrate = 9600) {
  *ctx = modbus_new_rtu(dev_name, baudrate, 'N', 8, 1);
  return 0;
}

class DemoData {
 public:
  static const constexpr char slave_name[] = "/tmp/ttyp0";
  static const constexpr char master_name[] = "/tmp/ptyp0";
  //static const constexpr char master_name[] = "/dev/ttyUSB0";
  static const constexpr uint8_t slave_address = 0x3;
};
