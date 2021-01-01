#pragma once
#include <Modbus/Modbus.h>
#include <cstdint>
#include <sys/time.h>
#include <Utilities/Crc.h>
#include <vector>
#include <string>

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
  snprintf(buff, sizeof(buff), "Unknown Command %d", static_cast<int>(function));
  return buff;
}


int64_t GetMicroSecondsSince(const timeval &first, const timeval &second) {
  return (second.tv_sec - first.tv_sec) * 1000000 +
         (second.tv_usec - first.tv_usec);
}

inline uint16_t crc16(const ArrayView<uint8_t> &array, std::size_t length) {
  assert(length <= array.size());
  return Utilities::crc16(array.data(), length);
}

