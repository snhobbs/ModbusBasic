
#include <Modbus/MappedRegisterDataStore.h>
#include <Modbus/Utilities.h>
#include <modbus.h>
#include <unistd.h>

#include <array>
#include <cassert>
#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>

#include "HoldingRegisterMappedDataStore.h"
#include "InputRegisterMappedDataStore.h"
template <typename T>
void PrintArray(const T& t) {
  for (auto pt : t) {
    std::cout << static_cast<int>(pt) << ",";
  }
  std::cout << std::endl;
}

template <typename T>
int ReadPrint(modbus_t* ctx, const int kAddressStart, int cnt) {
  T reg{};
  int rc = modbus_read_registers(ctx, kAddressStart, cnt, reg.data());
  if (rc == -1) {
    fprintf(stderr, "Read Error %s\n", modbus_strerror(errno));
    return -1;
  }
  assert(rc == cnt);
  PrintArray(reg);
  return 0;
}

inline int ConnectRtu(modbus_t** ctx, const char* dev_name,
                      const std::size_t baudrate = 9600) {
  *ctx = modbus_new_rtu(dev_name, baudrate, 'N', 8, 1);
  return 0;
}

int main(int argc, char** argv) {
  HoldingRegisters hregs_struct;
  HoldingRegistersWrapper hregs_wrapper{&hregs_struct};
  Modbus::MappedRegisterDataStore<HoldingRegistersWrapper> hregs{
      &hregs_wrapper};
  InputRegisters inregs_struct;
  InputRegistersWrapper inregs_wrapper{&inregs_struct};
  Modbus::MappedRegisterDataStore<InputRegistersWrapper> inregs{
      &inregs_wrapper};

  const char* dev_name = nullptr;
  uint8_t slave_address = 246;
  if (argc > 1) {
    dev_name = argv[1];
    if (argc > 2) {
      slave_address = atoi(argv[2]);
    }
  }
  printf("Device: %s\n", dev_name);
  printf("Address: %u\n", slave_address);
  modbus_t* ctx = nullptr;
  //  ConnectRtu(&ctx, DemoData::master_name);
  ConnectRtu(&ctx, dev_name);

  // Set the Modbus address of the remote slave
  modbus_set_slave(ctx, slave_address);  // DemoData::slave_address);

  if (modbus_connect(ctx) == -1) {
    fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
    modbus_free(ctx);
    return -1;
  }
  modbus_set_debug(ctx, slave_address);

#ifdef MODBUS_TIMEVAL
  struct timeval response_timeout;
  modbus_get_response_timeout(ctx, &response_timeout);

  response_timeout.tv_sec = 1;
  response_timeout.tv_usec = 0;
  modbus_set_response_timeout(ctx, &response_timeout);
  modbus_set_byte_timeout(ctx, &response_timeout);
#else
  modbus_set_response_timeout(ctx, 1, 0);
  modbus_set_byte_timeout(ctx, 1, 0);
#endif

  printf("Connection Successful\n");

  printf("\n\nWriting/Reading Input Registers\n\n");

  printf("Reading version\n");
  std::array<uint16_t, 32> read_buffer_version{};
  sleep(1);
  modbus_read_input_registers(ctx, 0, 32, read_buffer_version.data());
  {
    std::array<uint8_t, 32 * sizeof(uint16_t)> u8array{};
    Modbus::MakeRegistersToBytes(
        read_buffer_version,
        ArrayView<uint8_t>(u8array.size(), u8array.data()));
    inregs.SetFieldFromAddress(
        0, ArrayView<const uint8_t>(u8array.size(), u8array.data()));
    const auto value = inregs_wrapper.get_version();

    for (auto pt : value) {
      std::cout << pt << " ";
    }
    std::cout << "\n\n";
  }
  printf("Reading firmware_version\n");
  std::array<uint16_t, 32> read_buffer_firmware_version{};
  sleep(1);
  modbus_read_input_registers(ctx, 32, 32, read_buffer_firmware_version.data());
  {
    std::array<uint8_t, 32 * sizeof(uint16_t)> u8array{};
    Modbus::MakeRegistersToBytes(
        read_buffer_firmware_version,
        ArrayView<uint8_t>(u8array.size(), u8array.data()));
    inregs.SetFieldFromAddress(
        32, ArrayView<const uint8_t>(u8array.size(), u8array.data()));
    const auto value = inregs_wrapper.get_firmware_version();

    for (auto pt : value) {
      std::cout << pt << " ";
    }
    std::cout << "\n\n";
  }
  printf("Reading compile_date\n");
  std::array<uint16_t, 32> read_buffer_compile_date{};
  sleep(1);
  modbus_read_input_registers(ctx, 64, 32, read_buffer_compile_date.data());
  {
    std::array<uint8_t, 32 * sizeof(uint16_t)> u8array{};
    Modbus::MakeRegistersToBytes(
        read_buffer_compile_date,
        ArrayView<uint8_t>(u8array.size(), u8array.data()));
    inregs.SetFieldFromAddress(
        64, ArrayView<const uint8_t>(u8array.size(), u8array.data()));
    const auto value = inregs_wrapper.get_compile_date();

    for (auto pt : value) {
      std::cout << pt << " ";
    }
    std::cout << "\n\n";
  }
  printf("Reading compile_time\n");
  std::array<uint16_t, 32> read_buffer_compile_time{};
  sleep(1);
  modbus_read_input_registers(ctx, 96, 32, read_buffer_compile_time.data());
  {
    std::array<uint8_t, 32 * sizeof(uint16_t)> u8array{};
    Modbus::MakeRegistersToBytes(
        read_buffer_compile_time,
        ArrayView<uint8_t>(u8array.size(), u8array.data()));
    inregs.SetFieldFromAddress(
        96, ArrayView<const uint8_t>(u8array.size(), u8array.data()));
    const auto value = inregs_wrapper.get_compile_time();

    for (auto pt : value) {
      std::cout << pt << " ";
    }
    std::cout << "\n\n";
  }
  printf("Reading serial_number\n");
  std::array<uint16_t, 20> read_buffer_serial_number{};
  sleep(1);
  modbus_read_input_registers(ctx, 128, 20, read_buffer_serial_number.data());
  {
    std::array<uint8_t, 20 * sizeof(uint16_t)> u8array{};
    Modbus::MakeRegistersToBytes(
        read_buffer_serial_number,
        ArrayView<uint8_t>(u8array.size(), u8array.data()));
    inregs.SetFieldFromAddress(
        128, ArrayView<const uint8_t>(u8array.size(), u8array.data()));
    const auto value = inregs_wrapper.get_serial_number();

    for (auto pt : value) {
      std::cout << pt << " ";
    }
    std::cout << "\n\n";
  }
  printf("Reading fault_status\n");
  std::array<uint16_t, 1> read_buffer_fault_status{};
  sleep(1);
  modbus_read_input_registers(ctx, 148, 1, read_buffer_fault_status.data());
  {
    std::array<uint8_t, 1 * sizeof(uint16_t)> u8array{};
    Modbus::MakeRegistersToBytes(
        read_buffer_fault_status,
        ArrayView<uint8_t>(u8array.size(), u8array.data()));
    inregs.SetFieldFromAddress(
        148, ArrayView<const uint8_t>(u8array.size(), u8array.data()));
    const auto value = inregs_wrapper.get_fault_status();

    std::cout << value << "\n\n";
  }
  printf("Reading p5_micro_volts\n");
  std::array<uint16_t, 2> read_buffer_p5_micro_volts{};
  sleep(1);
  modbus_read_input_registers(ctx, 150, 2, read_buffer_p5_micro_volts.data());
  {
    std::array<uint8_t, 2 * sizeof(uint16_t)> u8array{};
    Modbus::MakeRegistersToBytes(
        read_buffer_p5_micro_volts,
        ArrayView<uint8_t>(u8array.size(), u8array.data()));
    inregs.SetFieldFromAddress(
        150, ArrayView<const uint8_t>(u8array.size(), u8array.data()));
    const auto value = inregs_wrapper.get_p5_micro_volts();

    std::cout << value << "\n\n";
  }
  printf("Reading p5_reading\n");
  std::array<uint16_t, 2> read_buffer_p5_reading{};
  sleep(1);
  modbus_read_input_registers(ctx, 152, 2, read_buffer_p5_reading.data());
  {
    std::array<uint8_t, 2 * sizeof(uint16_t)> u8array{};
    Modbus::MakeRegistersToBytes(
        read_buffer_p5_reading,
        ArrayView<uint8_t>(u8array.size(), u8array.data()));
    inregs.SetFieldFromAddress(
        152, ArrayView<const uint8_t>(u8array.size(), u8array.data()));
    const auto value = inregs_wrapper.get_p5_reading();

    std::cout << value << "\n\n";
  }
  printf("Reading p23_micro_volts\n");
  std::array<uint16_t, 2> read_buffer_p23_micro_volts{};
  sleep(1);
  modbus_read_input_registers(ctx, 154, 2, read_buffer_p23_micro_volts.data());
  {
    std::array<uint8_t, 2 * sizeof(uint16_t)> u8array{};
    Modbus::MakeRegistersToBytes(
        read_buffer_p23_micro_volts,
        ArrayView<uint8_t>(u8array.size(), u8array.data()));
    inregs.SetFieldFromAddress(
        154, ArrayView<const uint8_t>(u8array.size(), u8array.data()));
    const auto value = inregs_wrapper.get_p23_micro_volts();

    std::cout << value << "\n\n";
  }
  printf("Reading p23_reading\n");
  std::array<uint16_t, 2> read_buffer_p23_reading{};
  sleep(1);
  modbus_read_input_registers(ctx, 156, 2, read_buffer_p23_reading.data());
  {
    std::array<uint8_t, 2 * sizeof(uint16_t)> u8array{};
    Modbus::MakeRegistersToBytes(
        read_buffer_p23_reading,
        ArrayView<uint8_t>(u8array.size(), u8array.data()));
    inregs.SetFieldFromAddress(
        156, ArrayView<const uint8_t>(u8array.size(), u8array.data()));
    const auto value = inregs_wrapper.get_p23_reading();

    std::cout << value << "\n\n";
  }
  printf("Reading vlo_micro_volts\n");
  std::array<uint16_t, 2> read_buffer_vlo_micro_volts{};
  sleep(1);
  modbus_read_input_registers(ctx, 158, 2, read_buffer_vlo_micro_volts.data());
  {
    std::array<uint8_t, 2 * sizeof(uint16_t)> u8array{};
    Modbus::MakeRegistersToBytes(
        read_buffer_vlo_micro_volts,
        ArrayView<uint8_t>(u8array.size(), u8array.data()));
    inregs.SetFieldFromAddress(
        158, ArrayView<const uint8_t>(u8array.size(), u8array.data()));
    const auto value = inregs_wrapper.get_vlo_micro_volts();

    std::cout << value << "\n\n";
  }
  printf("Reading vlo_reading\n");
  std::array<uint16_t, 2> read_buffer_vlo_reading{};
  sleep(1);
  modbus_read_input_registers(ctx, 160, 2, read_buffer_vlo_reading.data());
  {
    std::array<uint8_t, 2 * sizeof(uint16_t)> u8array{};
    Modbus::MakeRegistersToBytes(
        read_buffer_vlo_reading,
        ArrayView<uint8_t>(u8array.size(), u8array.data()));
    inregs.SetFieldFromAddress(
        160, ArrayView<const uint8_t>(u8array.size(), u8array.data()));
    const auto value = inregs_wrapper.get_vlo_reading();

    std::cout << value << "\n\n";
  }
  printf("Reading thermistor_temp_spi\n");
  std::array<uint16_t, 2> read_buffer_thermistor_temp_spi{};
  sleep(1);
  modbus_read_input_registers(ctx, 162, 2,
                              read_buffer_thermistor_temp_spi.data());
  {
    std::array<uint8_t, 2 * sizeof(uint16_t)> u8array{};
    Modbus::MakeRegistersToBytes(
        read_buffer_thermistor_temp_spi,
        ArrayView<uint8_t>(u8array.size(), u8array.data()));
    inregs.SetFieldFromAddress(
        162, ArrayView<const uint8_t>(u8array.size(), u8array.data()));
    const auto value = inregs_wrapper.get_thermistor_temp_spi();

    std::cout << value << "\n\n";
  }
  printf("Reading thermistor_temp_mcu\n");
  std::array<uint16_t, 2> read_buffer_thermistor_temp_mcu{};
  sleep(1);
  modbus_read_input_registers(ctx, 164, 2,
                              read_buffer_thermistor_temp_mcu.data());
  {
    std::array<uint8_t, 2 * sizeof(uint16_t)> u8array{};
    Modbus::MakeRegistersToBytes(
        read_buffer_thermistor_temp_mcu,
        ArrayView<uint8_t>(u8array.size(), u8array.data()));
    inregs.SetFieldFromAddress(
        164, ArrayView<const uint8_t>(u8array.size(), u8array.data()));
    const auto value = inregs_wrapper.get_thermistor_temp_mcu();

    std::cout << value << "\n\n";
  }
  printf("Reading data_frequency\n");
  std::array<uint16_t, 2> read_buffer_data_frequency{};
  sleep(1);
  modbus_read_input_registers(ctx, 166, 2, read_buffer_data_frequency.data());
  {
    std::array<uint8_t, 2 * sizeof(uint16_t)> u8array{};
    Modbus::MakeRegistersToBytes(
        read_buffer_data_frequency,
        ArrayView<uint8_t>(u8array.size(), u8array.data()));
    inregs.SetFieldFromAddress(
        166, ArrayView<const uint8_t>(u8array.size(), u8array.data()));
    const auto value = inregs_wrapper.get_data_frequency();

    std::cout << value << "\n\n";
  }
  printf("Reading p3_3_micro_volts\n");
  std::array<uint16_t, 2> read_buffer_p3_3_micro_volts{};
  sleep(1);
  modbus_read_input_registers(ctx, 168, 2, read_buffer_p3_3_micro_volts.data());
  {
    std::array<uint8_t, 2 * sizeof(uint16_t)> u8array{};
    Modbus::MakeRegistersToBytes(
        read_buffer_p3_3_micro_volts,
        ArrayView<uint8_t>(u8array.size(), u8array.data()));
    inregs.SetFieldFromAddress(
        168, ArrayView<const uint8_t>(u8array.size(), u8array.data()));
    const auto value = inregs_wrapper.get_p3_3_micro_volts();

    std::cout << value << "\n\n";
  }
  printf("Reading p3_3_reading\n");
  std::array<uint16_t, 2> read_buffer_p3_3_reading{};
  sleep(1);
  modbus_read_input_registers(ctx, 170, 2, read_buffer_p3_3_reading.data());
  {
    std::array<uint8_t, 2 * sizeof(uint16_t)> u8array{};
    Modbus::MakeRegistersToBytes(
        read_buffer_p3_3_reading,
        ArrayView<uint8_t>(u8array.size(), u8array.data()));
    inregs.SetFieldFromAddress(
        170, ArrayView<const uint8_t>(u8array.size(), u8array.data()));
    const auto value = inregs_wrapper.get_p3_3_reading();

    std::cout << value << "\n\n";
  }

  printf("Writing/Reading Holding Registers\n");

  printf("Writing analog_output0\n");
  const std::array<uint16_t, 2> write_buffer_analog_output0{
      0xffff - 0,
      0xffff - 1,
  };
  modbus_write_registers(ctx, 0, 2, write_buffer_analog_output0.data());
  sleep(1);

  printf("Reading analog_output0\n");
  std::array<uint16_t, 2> read_buffer_analog_output0{};
  ReadPrint<decltype(read_buffer_analog_output0)>(ctx, 0, 2);
  sleep(1);

  printf("Writing analog_output1\n");
  const std::array<uint16_t, 2> write_buffer_analog_output1{
      0xffff - 0,
      0xffff - 1,
  };
  modbus_write_registers(ctx, 2, 2, write_buffer_analog_output1.data());
  sleep(1);

  printf("Reading analog_output1\n");
  std::array<uint16_t, 2> read_buffer_analog_output1{};
  ReadPrint<decltype(read_buffer_analog_output1)>(ctx, 2, 2);
  sleep(1);

  printf("Writing isp_mode\n");
  const std::array<uint16_t, 2> write_buffer_isp_mode{
      0xffff - 0,
      0xffff - 1,
  };
  modbus_write_registers(ctx, 4, 2, write_buffer_isp_mode.data());
  sleep(1);

  printf("Reading isp_mode\n");
  std::array<uint16_t, 2> read_buffer_isp_mode{};
  ReadPrint<decltype(read_buffer_isp_mode)>(ctx, 4, 2);
  sleep(1);

  printf("Writing slave_address_unlock\n");
  const std::array<uint16_t, 2> write_buffer_slave_address_unlock{
      0xffff - 0,
      0xffff - 1,
  };
  modbus_write_registers(ctx, 6, 2, write_buffer_slave_address_unlock.data());
  sleep(1);

  printf("Reading slave_address_unlock\n");
  std::array<uint16_t, 2> read_buffer_slave_address_unlock{};
  ReadPrint<decltype(read_buffer_slave_address_unlock)>(ctx, 6, 2);
  sleep(1);

  printf("Writing slave_address\n");
  const std::array<uint16_t, 1> write_buffer_slave_address{
      0xffff - 0,
  };
  modbus_write_registers(ctx, 8, 1, write_buffer_slave_address.data());
  sleep(1);

  printf("Reading slave_address\n");
  std::array<uint16_t, 1> read_buffer_slave_address{};
  ReadPrint<decltype(read_buffer_slave_address)>(ctx, 8, 1);
  sleep(1);

  printf("Writing clear_faults\n");
  const std::array<uint16_t, 2> write_buffer_clear_faults{
      0xffff - 0,
      0xffff - 1,
  };
  modbus_write_registers(ctx, 10, 2, write_buffer_clear_faults.data());
  sleep(1);

  printf("Reading clear_faults\n");
  std::array<uint16_t, 2> read_buffer_clear_faults{};
  ReadPrint<decltype(read_buffer_clear_faults)>(ctx, 10, 2);
  sleep(1);

  printf("Writing start_data_read\n");
  const std::array<uint16_t, 2> write_buffer_start_data_read{
      0xffff - 0,
      0xffff - 1,
  };
  modbus_write_registers(ctx, 12, 2, write_buffer_start_data_read.data());
  sleep(1);

  printf("Reading start_data_read\n");
  std::array<uint16_t, 2> read_buffer_start_data_read{};
  ReadPrint<decltype(read_buffer_start_data_read)>(ctx, 12, 2);
  sleep(1);

  modbus_close(ctx);
  modbus_free(ctx);
}
