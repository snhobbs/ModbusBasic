/* Copyright (C) 2020 Electrooptical Innovations
 * ----------------------------------------------------------------------
 * Project:      Modbus
 * Title:        test_RtuMaster.cpp
 * Description:  
 *
 * $Date:        13. May 2020
 * $Revision:    V.1.0.1
 * ----------------------------------------------------------------------
 */

#include "Crc.h"
#include <ArrayView/ArrayView.h>
#include <Modbus/Modbus.h>
#include <Modbus/ModbusRtu/ModbusRtuSlave.h>
//  #include <Modbus/ModbusRtu/ModbusRtuMaster.h>
#include <gtest/gtest.h>
#include <array>
#include <iostream>
#include <vector>
#include <cassert>
#include <cstdint>

#if 0
struct RtuMasterFixture {
  static const constexpr int kFrameOverhead =
      Modbus::RtuPacket::kHeaderLength + Modbus::RtuPacket::kFooterLength;
  static const constexpr std::size_t kAddress = 0xfa;
  static const constexpr std::size_t coil_count = 64;
  static const constexpr std::size_t holding_register_count = 64;
  Modbus::DeviceIdentifier device;
  static const constexpr std::size_t kSlaveCount = 1;
  static const constexpr std::array<std::size_t, kSlaveCount> slave_addresses{
      0xfa};
  Modbus::ProtocolRtuSlave<coil_count, holding_register_count> slave{
      &crc16, slave_addresses[0], device};
  Modbus::ProtocolRtuMaster<kSlaveCount> master{&crc16, slave_addresses};
  std::array<uint8_t, 9> data{1, 2, 3, 4, 5, 6, 7, 8, 0};
  RtuMasterFixture() {}
  ~RtuMasterFixture() {}
};
#endif
#if 0
TEST_FIXTURE(RtuSlaveFixture, SlaveFrameValidFunction) {
  for (auto func : Modbus::valid_functions) {
    std::array<uint8_t, 64> frame_data;
    Modbus::RtuFrame packet{0x0f, func, 0, ArrayView<uint8_t>{data.size(), data.data()}};
    ArrayView frame{data.size() + kFrameOverhead, frame_data.data()};
    prot.Frame(packet, &frame);
    CHECK(prot.FrameIsValid(frame));
  }
}

TEST_FIXTURE(RtuSlaveFixture, SlaveFrameInValidFunction) {
  for (uint16_t i = 0; i <= 0xff; i++) {
    std::array<uint8_t, 64> frame_data;
    Modbus::RtuFrame packet{0x0f, Modbus::valid_functions[0],0, ArrayView<uint8_t>{data.size(), data.data()}};

    ArrayView frame{data.size() + kFrameOverhead, frame_data.data()};
    prot.Frame(packet, &frame);
    frame[Modbus::RtuPacket::kFunctionLocation] = static_cast<uint16_t>(i);
    if (i == static_cast<uint8_t>(Modbus::valid_functions[0])) {
      CHECK(prot.FrameIsValid(frame));
    } else {
      CHECK(!prot.FrameIsValid(frame));
    }
  }
}

TEST_FIXTURE(RtuSlaveFixture, SlaveReadFrame) {
  std::array<uint8_t, 64> frame_data;
  Modbus::RtuFrame packet{0xff, Modbus::valid_functions[0], 0, ArrayView<uint8_t>{data.size(), data.data()}};
  ArrayView frame{data.size() + kFrameOverhead, frame_data.data()};
  prot.Frame(packet, &frame);
  std::array<uint8_t, 128> data_in{};
  Modbus::RtuFrame packet_read{
      0x0, Modbus::valid_functions.back(), 0, ArrayView<uint8_t>{data_in.size(), data_in.data()}};
  prot.ReadFrame(frame, &packet_read);
  for (std::size_t i = 0; i < data.size(); i++) {
    CHECK_EQUAL((int)packet.data_array[i], (int)packet_read.data_array[i]);
  }

  CHECK_EQUAL(packet.address, packet_read.address);
  CHECK(packet.function == packet_read.function);
}

TEST_FIXTURE(RtuSlaveFixture, SlaveAddressMatch) {
  for (std::size_t i = 0; i < 0xff; i++) {
    std::array<uint8_t, 9> data{1, 2, 3, 4, 5, 6, 7, 8, 0};
    Modbus::RtuFrame packet{static_cast<uint8_t>(i), Modbus::valid_functions[0], 0, ArrayView<uint8_t>{data.size(), data.data()}};
    if (i == kAddress) {
      CHECK(prot.FrameAddressMatch(packet));
    } else {
      CHECK(!prot.FrameAddressMatch(packet));
    }
  }
}

TEST_FIXTURE(RtuSlaveFixture, SlaveWriteCoils) {}

#if 0
TEST_FIXTURE(RtuSlaveFixture, SlaveValidateMessage) {
    ModbusRtuFrame packet{
        i,
        Modbus::valid_functions[0],
        ArrayView<uint8_t>{data.size(), data.data()}
    };
    CHECK(!prot.FrameAddressMatch(packet));
    }
}
#endif
#endif
