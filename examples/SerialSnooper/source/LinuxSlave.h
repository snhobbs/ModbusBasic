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
#include <Modbus/../../examples/posix/PosixSerial.h>
#include <Modbus/BitControl.h>
#include <Modbus/DataStore.h>
#include <Modbus/Modbus.h>
#include <Modbus/ModbusRtu/ModbusRtuSlave.h>
#include <Modbus/RegisterControl.h>
#include <Utilities/TypeConversion.h>
#include <sys/time.h>

#include <cassert>
#include <cstdint>
#include <vector>

#include "LinuxModbusTools.h"

inline const constexpr std::size_t kCoilCount = 125;
inline const constexpr std::size_t kRegisterCount = 100;
using CoilController =
    Modbus::CoilController<Modbus::BitFieldDataStore<kCoilCount>>;
using HoldingRegisterController = Modbus::HoldingRegisterController<
    Modbus::RegisterDataStore<kRegisterCount>>;
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

  Modbus::RegisterDataStore<kRegisterCount> holding_register_data_store;
  HoldingRegisterController hregs_{&holding_register_data_store};

  DiscreteInputController dins_;

  Modbus::RegisterDataStore<kRegisterCount> input_register_data_store;
  InputRegisterController inregs_{&input_register_data_store};

 private:
  static const constexpr uint8_t kSlaveAddress = 0x03;
  const char *const device_name;  // = "/tmp/ttyp0";

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
        // PrintPacketData();
        const auto &frame = GetFrameIn();
        if (slave_.FrameCrcIsValid(frame)) {
          Modbus::PrintPacketData(frame);
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
      : SlaveBase{&crc16, kSlaveAddress, coils_, hregs_, dins_, inregs_},
        device_name{port},
        iodev_{port, kBaudRate} {}
};
