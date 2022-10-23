/* Copyright (C) 2020 Electrooptical Innovations
 * ----------------------------------------------------------------------
 * Project:      Modbus
 * Title:        test_RtuSlave.cpp
 * Description:
 *
 * $Date:        13. May 2020
 * $Revision:    V.1.0.1
 * ----------------------------------------------------------------------
 */

#include <ArrayView/ArrayView.h>
#include <Modbus/DataStores/RegisterDataStore.h>
#include <Modbus/Modbus.h>
#include <Modbus/ModbusRtu/ModbusRtuSlave.h>
#include <Modbus/RegisterControl.h>
#include <gtest/gtest.h>

#include <array>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <vector>

#include "Crc.h"

namespace ModbusTests {

struct RtuSlaveFixture : public ::testing::Test {
  static const constexpr std::size_t kFrameOverhead =
      Modbus::Command::kHeaderLength + Modbus::Command::kFooterLength;
  static const constexpr uint8_t kSlaveAddress = 0xad;
  static const constexpr std::size_t holding_register_count = 64;

  using HoldingController =
      Modbus::HoldingRegisterController<Modbus::RegisterDataStore>;
  std::array<uint16_t, holding_register_count> registers{};
  Modbus::RegisterDataStore holding_map{registers.data(), registers.size()};
  HoldingController holding_register_controller{&holding_map};

  using InputController =
      Modbus::InputRegisterController<Modbus::RegisterDataStore>;
  std::array<uint16_t, holding_register_count> input_registers{};
  Modbus::RegisterDataStore input_map{registers.data(), registers.size()};
  InputController input_register_controller{&input_map};

  Modbus::ProtocolRtuSlave<HoldingController, InputController> slave{
      &crc16, kSlaveAddress, holding_register_controller,
      input_register_controller};
  static const constexpr std::size_t kDataLength = 9;
  std::array<uint8_t, kDataLength> data{1, 2, 3, 4, 5, 6, 7, 8, 0};

  void CheckFrame(Modbus::Frame &packet) {
    std::array<uint8_t, 256> input_data{};
    std::array<uint8_t, 256> output_data{};
    ArrayView<uint8_t> output_frame{GetRequiredPacketSize(packet),
                                    output_data.data()};

    Modbus::Frame packet_read{
        0, Modbus::Function::kNone, input_data.size(),
        ArrayView<uint8_t>{input_data.size(), input_data.data()}};
    Modbus::ProtocolRtu rtu{crc16};
    rtu.Frame(packet, &output_frame);
    rtu.ReadFrame(output_frame, &packet_read);
    //  EXPECT_TRUE(rtu.FrameCrcIsValid(packet));
    //  EXPECT_TRUE(rtu.FrameCrcIsValid(packet_read));

    if (packet.address == packet_read.address) {
      EXPECT_TRUE(slave.ValidateMessage(packet_read) ==
                  Modbus::Exception::kAck);
      EXPECT_EQ(static_cast<uint32_t>(slave.ValidateMessage(packet_read)),
                static_cast<uint32_t>(Modbus::Exception::kAck));
    }
  }
  void SetUp(void) {}
  void TearDown(void) {}
  RtuSlaveFixture() {}
  ~RtuSlaveFixture() {}
};

TEST_F(RtuSlaveFixture, SlaveRunCommand) {
  std::array<uint8_t, 64> frame_data;
  Modbus::Frame packet{kSlaveAddress, Modbus::GetValidFunctions()[0],
                       data.size(),
                       ArrayView<uint8_t>{data.size(), data.data()}};
  ArrayView<uint8_t> frame{data.size() + kFrameOverhead, frame_data.data()};
  Modbus::ProtocolRtu rtu{crc16};
  rtu.Frame(packet, &frame);
  std::array<uint8_t, 128> data_in{};
  Modbus::Frame packet_read{0x0, Modbus::GetValidFunctions().back(),
                            data_in.size(),
                            ArrayView<uint8_t>{data_in.size(), data_in.data()}};

  rtu.ReadFrame(frame, &packet_read);
  for (std::size_t i = 0; i < data.size(); i++) {
    EXPECT_EQ(static_cast<int>(packet.data_array[i]),
              static_cast<int>(packet_read.data_array[i]));
  }

  EXPECT_EQ(packet.address, packet_read.address);
  EXPECT_TRUE(packet.function == packet_read.function);
}

#if 0
TEST_F(RtuSlaveFixture, SlaveAddressMatch) {
  for (std::size_t i = 0; i < 0xff; i++) {
    Modbus::Frame packet{static_cast<uint8_t>(i), Modbus::GetValidFunctions()[0],
                            data.size(),
                            ArrayView<uint8_t>{data.size(), data.data()}};
    if (i == kSlaveAddress) {
      EXPECT_TRUE(slave.GetAddress() == packet.address);
    } else {
      EXPECT_TRUE(slave.GetAddress() == packet.address);
    }
  }
}
#endif

TEST_F(RtuSlaveFixture, WriteSingleHoldingRegisterCommand) {
  uint16_t address = 0;
  std::array<uint8_t, kDataLength> frame_data{};

  for (uint16_t reg = 0; reg < holding_register_count; reg++) {
    Modbus::Frame packet{
        kSlaveAddress, Modbus::Function::kNone, data.size(),
        ArrayView<uint8_t>{frame_data.size(), frame_data.data()}};
    packet.function = Modbus::Function::kWriteSingleHoldingRegister;

    Modbus::WriteSingleHoldingRegisterCommand::FillFrame(address++, 16,
                                                         &packet);
    CheckFrame(packet);
  }
}
TEST_F(RtuSlaveFixture, WriteMultipleHoldingRegistersCommand) {
  uint16_t address = 0;
  std::array<uint8_t, 1024> frame_data{};

  const std::size_t kRegisterCount = 16;
  for (uint16_t reg = 0; reg < holding_register_count - kRegisterCount; reg++) {
    const std::array<uint16_t, kRegisterCount> buff{0xffff};
    ArrayView<const uint16_t> register_data{buff.size(), buff.data()};
    Modbus::Frame packet{
        kSlaveAddress, Modbus::Function::kNone, frame_data.size(),
        ArrayView<uint8_t>{frame_data.size(), frame_data.data()}};
    packet.function = Modbus::Function::kWriteMultipleHoldingRegisters;

    Modbus::WriteMultipleHoldingRegistersCommand::FillFrame(
        address++, kRegisterCount, register_data, &packet);
    CheckFrame(packet);
  }
}
TEST_F(RtuSlaveFixture, ReadMultipleHoldingRegistersCommand) {
  uint16_t address = 0;
  const std::size_t kNumberOfRegisters = 16;
  std::array<uint8_t, 1024> frame_data{0, 1, 2, 3, 4, 5, 6, 7, 8};

  for (uint16_t reg = 0; reg < holding_register_count - kNumberOfRegisters;
       reg++) {
    Modbus::Frame packet{
        kSlaveAddress, Modbus::Function::kNone, data.size(),
        ArrayView<uint8_t>{frame_data.size(), frame_data.data()}};
    packet.function = Modbus::Function::kReadMultipleHoldingRegisters;

    Modbus::ReadMultipleHoldingRegistersCommand::FillFrame(
        address++, kNumberOfRegisters, &packet);
    CheckFrame(packet);
  }
}
//
//  Write the holding registers, send read command, check values are correct in
//  frame
TEST_F(RtuSlaveFixture, ReadHoldingValueCheck) {
  holding_register_controller.WriteRegister(0, 0xdead);
  holding_register_controller.WriteRegister(1, 0xdaed);
  holding_register_controller.WriteRegister(2, 0xfeed);
  holding_register_controller.WriteRegister(3, 0xbeef);

  std::array<uint8_t, 1024> frame_data{};
  Modbus::Frame frame{0xa,
                      Modbus::Function::kReadMultipleHoldingRegisters,
                      64,
                      {frame_data.size(), frame_data.data()}};
  Modbus::ReadMultipleHoldingRegistersCommand::FillFrame(0, 64, &frame);
  CheckFrame(frame);
  slave.RunCommand(frame);
  const auto resp = slave.GetResponse();

#if 0
  for (auto pt = 0; pt < resp.size()/*GetLength()*/; pt++) {
    printf("0x%x, ", resp.data()[pt]);
  }
#endif
  std::size_t offset = Modbus::ReadMultipleRegistersCommandBase::
      ResponsePacket::kHeaderSize;  //  address, function, bytes, data

  EXPECT_EQ(resp.at(offset++), 0xde);
  EXPECT_EQ(resp.at(offset++), 0xad);
  EXPECT_EQ(resp.at(offset++), 0xda);
  EXPECT_EQ(resp.at(offset++), 0xed);
  EXPECT_EQ(resp.at(offset++), 0xfe);
  EXPECT_EQ(resp.at(offset++), 0xed);
  EXPECT_EQ(resp.at(offset++), 0xbe);
  EXPECT_EQ(resp.at(offset++), 0xef);
}

}  //  namespace ModbusTests
