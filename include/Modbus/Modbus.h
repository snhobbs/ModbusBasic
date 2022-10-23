/* Copyright (C) 2020 Electrooptical Innovations
 * ----------------------------------------------------------------------
 * Project:      Modbus
 * Title:        Modbus.h
 * Description:  Shared definitions and base classes of Modbus library
 *
 * $Date:        13. May 2020
 * $Revision:    V.1.0.1
 * ----------------------------------------------------------------------
 */
#pragma once
#ifndef MODBUS_MODBUS_H_
#define MODBUS_MODBUS_H_

#include <ArrayView/ArrayView.h>

#include <array>
#include <cassert>
#include <cstdint>
/*
 * Central modbus definitions
 *  -> data types
 *  -> address spaces
 *  -> Functions
 *  -> Exceptions
 *
 *  Class Protocol
 *  Class RtuProtocol
 *      timer for packet delay
 *      crc calculator
 *  Class RtuProtocolMaster
 *  Class RtuProtocolSlave
 *
 * Framing will be handled by the protocol specific class,
 * slave and master will share that
 *
 * Slaves differ from masters by having addresses
 * Masters will need to keep a list of the known slaves
 * How do slaves get enumerated? -> Predetermined addresses
 * */

namespace Modbus {

const constexpr uint8_t kStatusResponseAddValue =
    0x80;  //  Add to function response when sending status code
enum class Exception : uint8_t {
  kNoException = 0,
  kIllegalFunction = 1,
  kIllegalDataAddress,
  kIllegalDataValue,
  kSlaveDeviceFailure,
  kAck,
  kSlaveDeviceBusy,
  kNak,
  kMemoryParityError,
  kGatewayPathUnavailable,
  kGatewayPathDeviceNoResponse,
  kCrcFailure,  //  Do not send this, internal use only
};

enum class AddressSpace : uint32_t {
  kCoil = 0,
  kDiscreteInput = 0x100000,
  kInputRegister = 0x300000,
  kHoldingRegister = 0x400000,
  kSystemStatus,
  kDeviceIdentifier,
  kUnmapped,
};

enum class Function : uint8_t {
  kReadCoils = 1,
  kReadDiscreteInputs = 2,
  kReadMultipleHoldingRegisters = 3,
  kReadInputRegisters = 4,
  kWriteSingleCoil = 5,
  kWriteSingleHoldingRegister = 6,
  kReadExceptionStatus = 7,
  kDiagnostic = 8,
  kGetComEventCounter = 11,
  kGetComEventLog = 12,
  kWriteMultipleCoils = 15,
  kWriteMultipleHoldingRegisters = 16,
  kReportSlaveId = 17,
  kReadFileRecord = 20,
  kWriteFileRecord = 21,
  kMaskWriteRegister = 22,
  kReadWriteMultipleRegisters = 23,
  kReadFifoQueue = 24,
  kReadDeviceIdentification = 43,

  kErrorReadCoils = static_cast<uint8_t>(kReadCoils) + kStatusResponseAddValue,
  kErrorReadDiscreteInputs =
      static_cast<uint8_t>(kReadDiscreteInputs) + kStatusResponseAddValue,
  kErrorReadMultipleHoldingRegisters =
      static_cast<uint8_t>(kReadMultipleHoldingRegisters) +
      kStatusResponseAddValue,
  kErrorReadInputRegisters =
      static_cast<uint8_t>(kReadInputRegisters) + kStatusResponseAddValue,
  kErrorWriteSingleCoil =
      static_cast<uint8_t>(kWriteSingleCoil) + kStatusResponseAddValue,
  kErrorWriteSingleHoldingRegister =
      static_cast<uint8_t>(kWriteSingleHoldingRegister) +
      kStatusResponseAddValue,
  kErrorReadExceptionStatus =
      static_cast<uint8_t>(kReadExceptionStatus) + kStatusResponseAddValue,
  kErrorDiagnostic =
      static_cast<uint8_t>(kDiagnostic) + kStatusResponseAddValue,
  kErrorGetComEventCounter =
      static_cast<uint8_t>(kGetComEventCounter) + kStatusResponseAddValue,
  kErrorGetComEventLog =
      static_cast<uint8_t>(kGetComEventLog) + kStatusResponseAddValue,
  kErrorWriteMultipleCoils =
      static_cast<uint8_t>(kWriteMultipleCoils) + kStatusResponseAddValue,
  kErrorWriteMultipleHoldingRegisters =
      static_cast<uint8_t>(kWriteMultipleHoldingRegisters) +
      kStatusResponseAddValue,
  kErrorReportSlaveId =
      static_cast<uint8_t>(kReportSlaveId) + kStatusResponseAddValue,
  kErrorReadFileRecord =
      static_cast<uint8_t>(kReadFileRecord) + kStatusResponseAddValue,
  kErrorWriteFileRecord =
      static_cast<uint8_t>(kWriteFileRecord) + kStatusResponseAddValue,
  kErrorMaskWriteRegister =
      static_cast<uint8_t>(kMaskWriteRegister) + kStatusResponseAddValue,
  kErrorReadWriteMultipleRegisters =
      static_cast<uint8_t>(kReadWriteMultipleRegisters) +
      kStatusResponseAddValue,
  kErrorReadFifoQueue =
      static_cast<uint8_t>(kReadFifoQueue) + kStatusResponseAddValue,
  kErrorReadDeviceIdentification =
      static_cast<uint8_t>(kReadDeviceIdentification) + kStatusResponseAddValue,

  kNone = 0xff
};

inline constexpr Modbus::Function GetErrorFunction(Modbus::Function function) {
  if (static_cast<uint8_t>(function) >= kStatusResponseAddValue) {
    return function;
  } else {
    return static_cast<Modbus::Function>(static_cast<uint8_t>(function) +
                                         kStatusResponseAddValue);
  }
}

enum class PacketState { kAddress, kFunction, kMeta, kData, kDone };

enum class CoilState : uint16_t {
  kOff = 0x0000,
  kOn = 0xff00,
  kInvalid = 0xffff
};

// const constexpr extern Modbus::Function valid_functions[18];
inline const constexpr std::array<const Modbus::Function, 19>
GetValidFunctions() {
  return {
      Modbus::Function::kReadCoils,
      Modbus::Function::kReadDiscreteInputs,
      Modbus::Function::kReadMultipleHoldingRegisters,
      Modbus::Function::kReadInputRegisters,
      Modbus::Function::kWriteSingleCoil,
      Modbus::Function::kWriteSingleHoldingRegister,
      Modbus::Function::kReadExceptionStatus,
      Modbus::Function::kDiagnostic,
      Modbus::Function::kGetComEventCounter,
      Modbus::Function::kGetComEventLog,
      Modbus::Function::kWriteMultipleCoils,
      Modbus::Function::kWriteMultipleHoldingRegisters,
      Modbus::Function::kReportSlaveId,
      Modbus::Function::kReadFileRecord,
      Modbus::Function::kWriteFileRecord,
      Modbus::Function::kMaskWriteRegister,
      Modbus::Function::kReadWriteMultipleRegisters,
      Modbus::Function::kReadFifoQueue,
      Modbus::Function::kReadDeviceIdentification,
  };
}

inline const constexpr std::array<const Modbus::Function, 4>
GetSupportedFunctions() {
  return {
      // Modbus::Function::kReadCoils,
      // Modbus::Function::kReadDiscreteInputs,
      Modbus::Function::kReadMultipleHoldingRegisters,
      Modbus::Function::kReadInputRegisters,
      // Modbus::Function::kWriteSingleCoil,
      Modbus::Function::kWriteSingleHoldingRegister,
      // Modbus::Function::kReadExceptionStatus,
      // Modbus::Function::kDiagnostic,
      // Modbus::Function::kGetComEventCounter,
      // Modbus::Function::kGetComEventLog,
      // Modbus::Function::kWriteMultipleCoils,
      Modbus::Function::kWriteMultipleHoldingRegisters,
      // Modbus::Function::kReportSlaveId,
      // Modbus::Function::kReadFileRecord,
      // Modbus::Function::kWriteFileRecord,
      // Modbus::Function::kMaskWriteRegister,
      // Modbus::Function::kReadWriteMultipleRegisters,
      // Modbus::Function::kReadFifoQueue,
      // Modbus::Function::kReadDeviceIdentification,
  };
}

inline constexpr Modbus::AddressSpace GetAddressSpaceFromFunction(
    Modbus::Function function) {
  switch (function) {
    case (Modbus::Function::kReadCoils):
      return Modbus::AddressSpace::kCoil;
    case (Modbus::Function::kWriteSingleCoil):
      return Modbus::AddressSpace::kCoil;
    case (Modbus::Function::kWriteMultipleCoils):
      return Modbus::AddressSpace::kCoil;
    case (Modbus::Function::kReadDiscreteInputs):
      return Modbus::AddressSpace::kDiscreteInput;
    case (Modbus::Function::kReadInputRegisters):
      return Modbus::AddressSpace::kInputRegister;
    case (Modbus::Function::kReadMultipleHoldingRegisters):
      return Modbus::AddressSpace::kHoldingRegister;
    case (Modbus::Function::kWriteSingleHoldingRegister):
      return Modbus::AddressSpace::kHoldingRegister;
    case (Modbus::Function::kWriteMultipleHoldingRegisters):
      return Modbus::AddressSpace::kHoldingRegister;
    case (Modbus::Function::kReadWriteMultipleRegisters):
      return Modbus::AddressSpace::kHoldingRegister;
    case (Modbus::Function::kMaskWriteRegister):
      return Modbus::AddressSpace::kHoldingRegister;
    case (Modbus::Function::kReadExceptionStatus):
      return Modbus::AddressSpace::kSystemStatus;
    case (Modbus::Function::kDiagnostic):
      return Modbus::AddressSpace::kSystemStatus;
    case (Modbus::Function::kGetComEventCounter):
      return Modbus::AddressSpace::kSystemStatus;
    case (Modbus::Function::kGetComEventLog):
      return Modbus::AddressSpace::kSystemStatus;
    case (Modbus::Function::kReportSlaveId):
      return Modbus::AddressSpace::kDeviceIdentifier;
    case (Modbus::Function::kReadFileRecord):
      return Modbus::AddressSpace::kUnmapped;
    case (Modbus::Function::kWriteFileRecord):
      return Modbus::AddressSpace::kUnmapped;
    case (Modbus::Function::kReadFifoQueue):
      return Modbus::AddressSpace::kUnmapped;
    case (Modbus::Function::kReadDeviceIdentification):
      return Modbus::AddressSpace::kDeviceIdentifier;
    case (Modbus::Function::kNone):
      return Modbus::AddressSpace::kUnmapped;
    default:
      assert(0);
      return Modbus::AddressSpace::kUnmapped;
  }
}

inline constexpr Modbus::Function GetFunction(uint8_t code) {
  for (auto i : Modbus::GetValidFunctions()) {
    if (code == static_cast<uint8_t>(i)) {
      return i;
    }
  }
  return Modbus::Function::kNone;
}

inline constexpr bool FunctionIsSupported(const Function function) {
  for (auto i : Modbus::GetSupportedFunctions()) {
    bool valid = false;
    if (function == i) {
      valid = true;
      break;
    }
    return valid;
  }
}

inline constexpr bool FunctionCodeIsValid(uint8_t code) {
  if (code > static_cast<uint8_t>(Function::kReadDeviceIdentification)) {
    return false;
  }
  for (auto pt : Modbus::GetValidFunctions()) {
    if (static_cast<uint8_t>(pt) == code) {
      return true;
    }
  }
  return false;
}

enum class PacketType { kCommand, kResponse, kError, kUnknown };

#if 0
class Packet {
  AddressSpace address_space;
  Function function_type;
  PacketType sub_type;

  /*
   * Return length of header, this is constant for each packet type
   * */
  virtual void get_header_length(void) const = 0;

  /*
   * Return length of data section, reads from the packet input
   * */
  virtual void get_data_length(const uint8_t* data, const size_t size) const = 0;
};
#endif

#if 1
struct Frame {
  uint8_t address = 0;
  Modbus::Function function{Modbus::Function::kNone};
  std::size_t data_length = 0;
  ArrayView<uint8_t> data_array{0, nullptr};

  void Reset(void) {
    address = 0;
    function = Modbus::Function::kNone;
    data_length = 0;
    for (auto &pt : data_array) {
      pt = 0;
    }
  }
  Frame(uint8_t addr, Modbus::Function func, std::size_t length,
        ArrayView<uint8_t> arrayview)
      : address{addr},
        function{func},
        data_length{length},
        data_array{arrayview.size(), arrayview.data()} {}

  explicit Frame(ArrayView<uint8_t> arrayview)
      : data_array{arrayview.size(), arrayview.data()} {}

  Frame(void) {}
};

class Response {
  static const std::size_t kMaxFrameLength = 256;
  std::size_t length_ = 0;
  std::array<uint8_t, kMaxFrameLength> data_{};
  bool ready_ = false;

 public:
  std::size_t GetLength(void) const { return length_; }
  void SetLength(size_t length) { length_ = length; }
  bool IsReady(void) const { return ready_; }
  void SetReady(bool ready) { ready_ = ready; }
  uint8_t *data(void) { return data_.data(); }
  const uint8_t *data(void) const { return data_.data(); }
  constexpr std::size_t size(void) const { return data_.size(); }
  const uint8_t &at(std::size_t index) const {
    assert(index <= size());
    return data_[index < size() ? index : 0];
  }
  const uint8_t &operator[](std::size_t index) const { return at(index); }
  uint8_t &operator[](std::size_t index) {
    assert(index <= size());
    return data_[index < size() ? index : 0];
  }
  ArrayView<uint8_t> GetArrayView(void) {
    return ArrayView<uint8_t>{GetLength(), data()};
  }
  const ArrayView<const uint8_t> GetArrayView(void) const {
    return ArrayView<const uint8_t>{GetLength(), data()};
  }

  const uint8_t *begin(void) const { return data_.begin(); }
  const uint8_t *end(void) const { return begin() + GetLength(); }
  uint8_t *begin(void) { return data_.begin(); }
  uint8_t *end(void) { return data_.begin() + GetLength(); }
  void Reset(void) {
    for (uint8_t &pt : data_) {
      pt = 0;
    }
    SetLength(0);
    SetReady(false);
  }

  constexpr Response(void) {}
};

struct Command {
  static const constexpr uint32_t kHeaderLength = 2 * sizeof(uint8_t);
  static const constexpr uint32_t kFooterLength = 1 * sizeof(uint16_t);
  struct CommandPacket {
    static const constexpr std::size_t kSlaveAddress = 0;
    static const constexpr std::size_t kFunction = 1;
    static const constexpr std::size_t kHeaderEnd = kFunction;
  };
  struct ResponsePacket {
    static const constexpr std::size_t kSlaveAddress =
        CommandPacket::kSlaveAddress;
    static const constexpr std::size_t kFunction = CommandPacket::kFunction;
    static const constexpr std::size_t kHeaderEnd = CommandPacket::kHeaderEnd;
  };
};

class Protocol {};

#endif
}  //  namespace Modbus
#endif  //  MODBUS_MODBUS_H_
