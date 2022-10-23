#pragma once

#include <Modbus/DataCommand.h>
#include <Modbus/Modbus.h>
#include <Modbus/RegisterControl.h>
#include <Utilities/Crc.h>
#include <sys/time.h>

#include <cstdint>
#include <cstdio>  // standard input / output functions
#include <string>

inline int64_t GetMicroSecondsSince(const timeval &first,
                                    const timeval &second) {
  return (second.tv_sec - first.tv_sec) * 1000000 +
         (second.tv_usec - first.tv_usec);
}

inline uint16_t crc16(const ArrayView<uint8_t> &array, std::size_t length) {
  assert(length <= array.size());
  return Utilities::crc16(array.data(), length);
}

inline timeval GetTimeStamp(void) {
  timeval tv;
  gettimeofday(&tv, NULL);
  return tv;
}

namespace Modbus {
inline std::string GetFunctionName(const Modbus::Function function) {
  switch (function) {
    case Modbus::Function::kReadCoils:
      return "Read Coils";
    case Modbus::Function::kReadDiscreteInputs:
      return "Read Discrete Inputs";
    case Modbus::Function::kReadMultipleHoldingRegisters:
      return "Read Multiple Holding Registers";
    case Modbus::Function::kReadInputRegisters:
      return "Read Input Registers";
    case Modbus::Function::kWriteSingleCoil:
      return "Read Single Coil";
    case Modbus::Function::kWriteSingleHoldingRegister:
      return "Write Single Holding Register";
    case Modbus::Function::kWriteMultipleHoldingRegisters:
      return "Write Multiple Holding Registers";
    case Modbus::Function::kWriteMultipleCoils:
      return "Write Multiple Coils";
    case Modbus::Function::kReadDeviceIdentification:
      return "Read Device Information";
    default:
      break;
  }
  char buff[256]{};
  snprintf(buff, sizeof(buff), "Unknown Command %d",
           static_cast<int>(function));
  return buff;
}

inline void PrintPacketData(const Modbus::Frame &frame) {
  printf("Slave Address %d: %s ", frame.address,
         GetFunctionName(frame.function).c_str());
  switch (frame.function) {
    case Modbus::Function::kReadMultipleHoldingRegisters: {
      const uint16_t address =
          Modbus::DataCommand::ReadAddressStart(frame.data_array);
      const uint16_t register_count =
          Modbus::ReadMultipleHoldingRegistersCommand::ReadRegisterCount(
              frame.data_array);
      printf("Address: 0x%x, Count: %d", address, register_count);
      break;
    }
    case Modbus::Function::kWriteMultipleHoldingRegisters: {
      const uint16_t address =
          Modbus::DataCommand::ReadAddressStart(frame.data_array);
      const uint16_t register_count =
          Modbus::WriteMultipleHoldingRegistersCommand::ReadRegisterCount(
              frame.data_array);
      printf("Address: 0x%x, Count: %d", address, register_count);
      break;
    }
    default:
      break;
  }

  printf(" [");
  for (std::size_t i = 0; i < frame.data_length; i++) {
    printf("0x%x ", frame.data_array[i]);
  }
  printf("]");
}

}  //  namespace Modbus
