#include <modbus.h>
#include <unistd.h>

#include <array>
#include <cassert>
#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>

#include "DemoHeader.h"
template <typename T>
void PrintArray(const T& t) {
  for (auto pt : t) {
    std::cout << static_cast<int>(pt) << ",";
  }
  std::cout << std::endl;
}

template <typename T>
void ZeroArray(T& t) {
  for (auto& pt : t) {
    pt = 0;
  }
}
template <typename T>
int ReadPrint(modbus_t* ctx, const int kAddressStart, int cnt) {
  T reg{};
  int rc = modbus_read_registers(ctx, kAddressStart, cnt, reg.data());
  if (rc == -1) {
    fprintf(stderr, "%s\n", modbus_strerror(errno));
    return -1;
  }
  assert(rc == cnt);
  sleep(1);
  PrintArray(reg);
  return 0;
}

template <typename T>
int ReadPrintCoils(modbus_t* ctx, const int kAddressStart, int cnt) {
  T reg{};
  int rc = modbus_read_bits(ctx, kAddressStart, cnt, reg.data());
  if (rc == -1) {
    fprintf(stderr, "%s\n", modbus_strerror(errno));
    return -1;
  }
  assert(rc == cnt);
  sleep(1);
  PrintArray(reg);
  return 0;
}

int main(int argc, char** argv) {
  std::string port = "/dev/ttyUSB0";
  uint8_t slave_address = 3;
  if (argc > 1) {
    port = argv[1];
  }
  if (argc > 2) {
    slave_address = atoi(argv[2]);
  }
  printf("%s, slave %d\n", port.c_str(), slave_address);
  modbus_t* ctx = nullptr;
  //  ConnectRtu(&ctx, DemoData::master_name);
  ConnectRtu(&ctx, port.c_str());

  // Set the Modbus address of the remote slave
  modbus_set_slave(ctx, slave_address);  // DemoData::slave_address);

  if (modbus_connect(ctx) == -1) {
    fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
    modbus_free(ctx);
    return -1;
  }
  fprintf(stdout, "Connection Successful\n");

  const int kNumRegisters = 32;
  const std::array<uint16_t, kNumRegisters> zeros{};
  const int kAddressStart = 0;

#if 1
  printf("Write Zeros 10-14\n");
  modbus_write_registers(ctx, kAddressStart, kNumRegisters, zeros.data());
  printf("Read Registers 10-14\n");
  ReadPrint<decltype(zeros)>(ctx, kAddressStart, zeros.size());
  printf("Write Individual Registers 10, 11, & 12\n");
  modbus_write_register(ctx, kAddressStart, 9);
  modbus_write_register(ctx, kAddressStart + 1, 10);
  modbus_write_register(ctx, kAddressStart + 2, 11);
  sleep(2);
  printf("Read Registers 10-14\n");
  ReadPrint<decltype(zeros)>(ctx, kAddressStart, zeros.size());

  sleep(2);
  printf("Write Zeros 10-14\n");
  modbus_write_registers(ctx, kAddressStart, kNumRegisters, zeros.data());
  printf("Read Registers 10-14\n");
  ReadPrint<decltype(zeros)>(ctx, kAddressStart, zeros.size());

  sleep(2);
  modbus_write_registers(ctx, kAddressStart, kNumRegisters, src_reg0.data());
  ReadPrint<decltype(zeros)>(ctx, kAddressStart, zeros.size());

  modbus_write_registers(ctx, kAddressStart, kNumRegisters, src_reg1.data());
  ReadPrint<decltype(zeros)>(ctx, kAddressStart, zeros.size());
#endif

  modbus_close(ctx);
  modbus_free(ctx);
}
