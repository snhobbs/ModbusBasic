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
inline const constexpr std::size_t kRegisterCount = 1024;
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
  //  Modbus::BitFieldDataStore<kCoilCount> coil_data_store;
  CoilController coils_;

  Modbus::RegisterDataStore<kRegisterCount> holding_register_data_store;
  HoldingRegisterController hregs_{&holding_register_data_store};

  DiscreteInputController dins_;

  Modbus::RegisterDataStore<kRegisterCount> input_register_data_store;
  InputRegisterController inregs_{&input_register_data_store};

  static const constexpr uint8_t kSlaveAddress = 0x03;

  static const constexpr int kBaudRateHz = 9600;
  static const constexpr speed_t kBaudRate = B9600;
  static const constexpr int kCharacterClocks = 8 + 1;

  const char *const device_name;  // = "/tmp/ttyp0";
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

  void ProcessPacket(void) {
    ProcessMessage();

    const auto &frame = GetFrameIn();
    Modbus::PrintPacketData(frame);
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
    const bool character_timeout =
        GetMicroSecondsSince(last_character_time_, timestamp) >=
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
      : SlaveBase{&crc16, kSlaveAddress, coils_, hregs_, dins_, inregs_},
        device_name{port},
        iodev_{port, kBaudRate} {}
};
