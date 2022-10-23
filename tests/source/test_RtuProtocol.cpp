/* Copyright (C) 2020 Electrooptical Innovations
 * ----------------------------------------------------------------------
 * Project:      Modbus
 * Title:        test_RtuProtocol.cpp
 * Description:
 *
 * $Date:        13. May 2020
 * $Revision:    V.1.0.1
 * ----------------------------------------------------------------------
 */

#include <ArrayView/ArrayView.h>
#include <Modbus/DataStores/RegisterDataStore.h>
#include <Modbus/Modbus.h>
#include <Modbus/ModbusRtu/ModbusRtuProtocol.h>
#include <Modbus/RegisterControl.h>
#include <gtest/gtest.h>

#include <array>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <vector>

#include "Crc.h"

namespace ModbusTests {
static int kFrameOverhead =
    Modbus::Command::kHeaderLength + Modbus::Command::kFooterLength;

TEST(CalculateBytesRemaining, ReadHoldingRegisters) {
  std::array<uint8_t, 1024> data{};
  Modbus::Frame frame{0x0f, Modbus::Function::kWriteMultipleHoldingRegisters,
                      data.size(),
                      ArrayView<uint8_t>{data.size(), data.data()}};
  for (uint8_t i = 1; i > 0; i++) {
    frame.data_array[Modbus::WriteMultipleHoldingRegistersCommand::
                         CommandPacket::kNumberOfDataBytes] = i;
    EXPECT_EQ(CalculateBytesRemaining(frame), i);
  }
}

TEST(Response, copy) {
  Modbus::Response ra{};
  Modbus::Response rb{};
  ra.SetLength(100);
  ra.SetReady(true);
  for (uint8_t pt = 0; pt < ra.GetLength(); pt++) {
    ra.data()[pt] = pt;
  }

  rb = ra;
  EXPECT_EQ(rb.GetLength(), ra.GetLength());
  EXPECT_EQ(rb.IsReady(), ra.IsReady());

  for (uint8_t pt = 0; pt < ra.GetLength(); pt++) {
    EXPECT_EQ(ra.at(pt), rb.at(pt));
  }
}

struct RtuProtocolFixture : public ::testing::Test {
  Modbus::ProtocolRtu prot{&crc16};
  std::array<uint8_t, 9> data{1, 2, 3, 4, 5, 6, 7, 8, 0};
  void SetUp(void) {}
  void TearDown(void) {}
};

TEST_F(RtuProtocolFixture, FrameCRCIsValidFunction) {
  for (auto func : Modbus::GetValidFunctions()) {
    std::array<uint8_t, 64> frame_data;
    Modbus::Frame packet{0x0f, func, data.size(),
                         ArrayView<uint8_t>{data.size(), data.data()}};
    ArrayView frame{data.size() + kFrameOverhead, frame_data.data()};
    prot.Frame(packet, &frame);
    EXPECT_TRUE(prot.FrameCrcIsValid(frame));
  }
}
TEST_F(RtuProtocolFixture, FrameCRCIsValid_false) {
  for (uint32_t i = 0; i <= 0xff; i++) {
    std::array<uint8_t, 64> frame_data;
    Modbus::Frame packet{0x0f, Modbus::GetValidFunctions()[0], data.size(),
                         ArrayView<uint8_t>{data.size(), data.data()}};

    ArrayView frame{data.size() + kFrameOverhead, frame_data.data()};
    prot.Frame(packet, &frame);
    frame[Modbus::Command::CommandPacket::kFunction] = static_cast<uint8_t>(i);
    if (i == static_cast<uint8_t>(Modbus::GetValidFunctions()[0])) {
      EXPECT_TRUE(prot.FrameCrcIsValid(frame));
    } else {
      EXPECT_TRUE(!prot.FrameCrcIsValid(frame));
    }
  }
}
TEST_F(RtuProtocolFixture, ReadFrame) {
  std::array<uint8_t, 64> frame_data;
  Modbus::Frame packet{0xff, Modbus::GetValidFunctions()[0], data.size(),
                       ArrayView<uint8_t>{data.size(), data.data()}};
  ArrayView frame{data.size() + kFrameOverhead, frame_data.data()};
  prot.Frame(packet, &frame);
  std::array<uint8_t, 128> data_in{};
  Modbus::Frame packet_read{0x0, Modbus::GetValidFunctions().back(), 0,
                            ArrayView<uint8_t>{data_in.size(), data_in.data()}};
  prot.ReadFrame(frame, &packet_read);
  for (std::size_t i = 0; i < data.size(); i++) {
    EXPECT_EQ((int)packet.data_array[i], (int)packet_read.data_array[i]);
  }

  EXPECT_EQ(packet.address, packet_read.address);
  EXPECT_TRUE(packet.function == packet_read.function);
}
}  //  namespace ModbusTests
