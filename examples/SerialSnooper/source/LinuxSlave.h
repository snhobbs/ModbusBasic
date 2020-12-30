/* Copyright (C) 2020 Electrooptical Innovations
 * ----------------------------------------------------------------------
 * Project:      Modbus
 * Title:        LinuxSlave.h
 * Description:  Linux modbus slave
 *
 * $Date:        13. May 2020
 * $Revision:    V.1.0.1
 * ----------------------------------------------------------------------
 */

#pragma once
#include <IODevice/IODevice.h>
#include <Modbus/BitControl.h>
#include <Modbus/DefaultSlaveDeviceIdentifier.h>
#include <Modbus/Modbus.h>
#include <Modbus/ModbusRtu/ModbusRtuProtocol.h>
#include <Modbus/ModbusRtu/ModbusRtuSlave.h>
#include <Modbus/RegisterControl.h>
#include <Modbus/DataStore.h>
#include <RingBuffer/RingBuffer.h>
#include <Utilities/TypeConversion.h>
#include <Utilities/Crc.h>
#include <cassert>
#include <cstdint>
#include <cstdio> // standard input / output functions
#include <cstdlib>
#include <cstring> // string function definitions
#include <fcntl.h> // File control definitions
#include <iostream>
#include <sys/time.h>
#include <unistd.h>  // UNIX standard function definitions
#include "PosixSerial.h" // examples/posix/PosixSerial.h

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

inline const constexpr std::size_t kCoilCount = 125;
inline const constexpr std::size_t kRegisterCount = 100;
using CoilController =
    Modbus::CoilController<Modbus::BitFieldDataStore<kCoilCount>>;
using HoldingRegisterController =
  Modbus::HoldingRegisterController<Modbus::RegisterDataStore<kRegisterCount>>;
using DiscreteInputController =
    Modbus::DiscreteInputController<Modbus::BitFieldDataStore<kCoilCount>>;
using InputRegisterController =
    Modbus::InputRegisterController<Modbus::RegisterDataStore<kRegisterCount>>;
using SlaveBase =
    Modbus::ProtocolRtuSlave<CoilController, HoldingRegisterController,
                             DiscreteInputController, InputRegisterController>;

class LinuxSlave : public SlaveBase {
 public:
  CoilController coils_;
  HoldingRegisterController hregs_;
  DiscreteInputController dins_;
  InputRegisterController inregs_;

 private:
  static const constexpr uint8_t kSlaveAddress = 0x03;
  static const constexpr SlaveDeviceIdentifier identity{};
  const char *const device_name; // = "/tmp/ttyp0";

  static const constexpr int kBaudRateHz = 9600;
  static const constexpr speed_t kBaudRate = B9600;
  static const constexpr int kCharacterClocks = 8 + 1;

  timeval last_character_time_ = GetTimeStamp();
  static const constexpr int64_t kFrameDelay_us = static_cast<int64_t>(
      (1e6 * (kCharacterClocks * 3.5)) / (static_cast<double>(kBaudRateHz)));
  UartController iodev_;
  int byte_counter_ = 0;

  void RunIO(void) {
    iodev_.SendTxBuff();
    iodev_.ReadIntoRxBuff();
  }

  timeval GetTimeStamp(void) {
    timeval tv;
    gettimeofday(&tv, NULL);
    return tv;
  }

#if 0
  void ProcessMessage(void) {
    Modbus::Exception exception = ValidateMessage(framein_);
    if (GetAddress() == framein_.address) {
      if (exception == Modbus::Exception::kAck) {
    	RunCommand(framein_);
      } else {
    	SendErrorResponse(framein_, exception);
      }
    }
  }
#endif

  void PrintPacketData(void) {
    const auto& frame = GetFrameIn();
    printf("\nSlave Address %d: %s ", frame.address, GetFunctionName(frame.function).c_str());
    switch (frame.function) {
    case Modbus::Function::kReadMultipleHoldingRegisters: {
      const uint16_t address = Modbus::DataCommand::ReadAddressStart(frame.data_array);
      const uint16_t register_count = Modbus::ReadMultipleHoldingRegistersCommand::ReadRegisterCount(frame.data_array);
      printf("Address: 0x%x, Count: %d", address, register_count);
      break;
    }
    case Modbus::Function::kWriteMultipleHoldingRegisters: {
      const uint16_t address = Modbus::DataCommand::ReadAddressStart(frame.data_array);
      const uint16_t register_count = Modbus::WriteMultipleHoldingRegistersCommand::ReadRegisterCount(frame.data_array);
      printf("Address: 0x%x, Count: %d", address, register_count);
      break;
    }
    }

    printf(" [");
    for (std::size_t i = 0; i < frame.data_length; i++) {
      printf("0x%x ", frame.data_array[i]);
    }
    printf("]\n");
 
  }
  void ProcessPacket(void) {
    ProcessMessage();
    Reset();
#if 0
    case Modbus::Function::kReadMultipleHoldingRegisters: {
      const uint16_t address = Modbus::DataCommand::ReadAddressStart(frame.data_array);
      const uint16_t register_count = Modbus::ReadMultipleHoldingRegistersCommand::ReadRegisterCount(frame.data_array);
      printf("Address: 0x%x, Count: %d", address, register_count);
      break;
    }
    case Modbus::Function::kWriteMultipleHoldingRegisters: {
      const uint16_t address = Modbus::DataCommand::ReadAddressStart(frame.data_array);
      const uint16_t register_count = Modbus::WriteMultipleHoldingRegistersCommand::ReadRegisterCount(frame.data_array);
      printf("Address: 0x%x, Count: %d", address, register_count);
      break;
    }
#endif
  }

  bool PacketIsReady(const timeval &timestamp) const {
    return GetMicroSecondsSince(last_character_time_, timestamp) >=
           kFrameDelay_us;
  }

  void CheckForCompletedPacket(const timeval &timestamp) {
    if (PacketIsReady(timestamp)) {
      if ((GetContext().GetState() == Modbus::PacketState::kData)) {
        //PrintPacketData();
        const auto& frame = GetFrameIn();
        if (slave_.FrameCrcIsValid(frame)) {
          PrintPacketData();
          ProcessPacket();
        } else {
          // printf("Crc invalid\n");
        }
      }
      Reset();
    }
  }

 public:
  void Run(void) {
    // sleep(0.005);
    RunIO();
    CheckForCompletedPacket(GetTimeStamp());
    if (!iodev_.rxEmpty()) {
      last_character_time_ = GetTimeStamp();
      uint8_t data = 0;
      iodev_.read(&data, 1);
      slave_.ProcessCharacter(data);
    }
  }
  explicit LinuxSlave(const char *const port)
      : SlaveBase{&crc16, kSlaveAddress, identity.GetDeviceIdentifier(),
                  coils_, hregs_,        dins_,
                  inregs_},
        device_name{port}, iodev_{port, kBaudRate} {}
};
