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
#include "LinuxModbusTools.h"
#include <Modbus/../../examples/posix/PosixSerial.h>
#include <Modbus/Modbus.h>
#include <Modbus/BitControl.h>
#include <Modbus/DataStore.h>
#include <Modbus/DefaultSlaveDeviceIdentifier.h>
#include <Modbus/ModbusRtu/ModbusRtuProtocol.h>
#include <Modbus/ModbusRtu/ModbusRtuSlave.h>
#include <Modbus/RegisterControl.h>
#include <RingBuffer/RingBuffer.h>
#include <Utilities/TypeConversion.h>
#include <cassert>
#include <cstdint>
#include <cstdio> // standard input / output functions
#include <cstdlib>
#include <cstring> // string function definitions
#include <fcntl.h> // File control definitions
#include <iostream>
#include <sys/time.h>
#include <unistd.h>  // UNIX standard function definitions
#include <vector>
#include <string>

inline const constexpr std::size_t kCoilCount = 125;
inline const constexpr std::size_t kRegisterCount = 1024;
using CoilController =
    Modbus::CoilController<Modbus::BitFieldDataStore<kCoilCount>>;
using HoldingRegisterController =
  Modbus::HoldingRegisterController<Modbus::RegisterDataStore<kRegisterCount>>;
using DiscreteInputController =
    Modbus::DiscreteInputController<Modbus::BitFieldDataStore<kCoilCount>>;
using InputRegisterController =
    Modbus::InputRegisterController<Modbus::RegisterDataStore<kRegisterCount>>;

using SlaveBase =
      Modbus::ProtocolRtuSlave<CoilController, HoldingRegisterController, DiscreteInputController, InputRegisterController>;

class LinuxMaster : public SlaveBase {
 public:
  CoilController coils_;
  HoldingRegisterController hregs_;
  DiscreteInputController dins_;
  InputRegisterController inregs_;
  SlaveDeviceIdentifier device_identifier_;

  static const constexpr uint8_t kSlaveAddress = 0x03;

  static const constexpr int kBaudRateHz = 9600;
  static const constexpr speed_t kBaudRate = B9600;
  static const constexpr int kCharacterClocks = 8 + 1;

  const char *const device_name; // = "/tmp/ttyp0";
  timeval last_character_time_ = GetTimeStamp();
  static const constexpr int64_t kFrameDelay_us =
      (1e6 * (kCharacterClocks * 3.5)) / (static_cast<double>(kBaudRateHz));
  UartController iodev_;
  int byte_counter_ = 0;

  timeval GetTimeStamp(void) {
    timeval tv;
    gettimeofday(&tv, NULL);
    return tv;
  }

  void PrintPacketData(const Modbus::Frame& frame) {
    printf("Slave Address %d: %s ", frame.address, GetFunctionName(frame.function).c_str());
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
    default:
      break;
    }

    printf(" [");
    for (std::size_t i = 0; i < frame.data_length; i++) {
      printf("0x%x ", frame.data_array[i]);
    }
    printf("]");
 
  }

  void ProcessPacket(void) {
    ProcessMessage();

    const auto& frame = GetFrameIn();
      PrintPacketData(frame);
      printf("\n");
#if 0
    if (frame.address == 246 || frame.function == Modbus::Function::kWriteMultipleHoldingRegisters) {
      printf("\n");
      PrintPacketData(frame);
      printf("\n");
    } else {
      printf(".");
      fflush(stdout);
    }
#endif
    if (GetResponseValid()) {
      iodev_.write(GetResponse().data(), GetResponse().GetLength());
      printf("Response: [");
      const auto response = GetResponse();
      std::size_t cnt = 0;
      for (auto pt : response) {
        cnt++;
        printf("%d ", pt);
      }
      printf("]\n");
      assert(cnt == response.GetLength());
    }
    Reset();
  }

  bool RxCharacterTimeout(const timeval &timestamp) const {
    const bool character_timeout = GetMicroSecondsSince(last_character_time_, timestamp) >=
           10 * kFrameDelay_us;
    return character_timeout;
  }

 public:
  void Run(void) {
    // sleep(0.005);

    iodev_.SendTxBuff();
    iodev_.ReadIntoRxBuff();

    if (slave_.ctx_.PacketReceived()) {
      //  check slave address
      ProcessPacket();
    }
    if (!iodev_.rxEmpty()) {
      last_character_time_ = GetTimeStamp();
      uint8_t data = 0;
      iodev_.read(&data, 1);
      slave_.ProcessCharacter(data);
    } else if (RxCharacterTimeout(GetTimeStamp())) {
      Reset();
    }
  }

  explicit LinuxSlave(const char *const port)
      : SlaveBase{&crc16, kSlaveAddress, device_identifier_.GetDeviceIdentifier(), coils_,
                  hregs_, dins_, inregs_},
        device_name{port}, iodev_{port, kBaudRate} {}
};
