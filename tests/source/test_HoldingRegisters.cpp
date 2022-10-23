/* Copyright (C) 2020 Electrooptical Innovations
 * ----------------------------------------------------------------------
 * Project:      Modbus
 * Title:        test_HoldingRegisters.cpp
 * Description:
 *
 * $Date:        13. May 2020
 * $Revision:    V.1.0.1
 * ----------------------------------------------------------------------
 */

#include <Modbus/DataStores/RegisterDataStore.h>
#include <Modbus/Modbus.h>
#include <Modbus/RegisterControl.h>

#include "Crc.h"
//#include <Modbus/ModbusRtu/ModbusRtu.h>
#include <ArrayView/ArrayView.h>
#include <gtest/gtest.h>

#include <array>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <vector>

namespace ModbusTests {
static const constexpr std::size_t kRegisters = 69;

struct HoldingRegisterDataFixture : public ::testing::Test {
  std::array<uint16_t, kRegisters> registers{};
  Modbus::RegisterDataStore dc{registers.data(), registers.size()};
  void SetUp(void) {}
  void TearDown(void) {}
};
TEST_F(HoldingRegisterDataFixture, RegisterReadWrite) {
  for (std::size_t i = 0; i < dc.GetSize(); i++) {
    std::size_t address = dc.GetAddressStart() + 1;
    const uint16_t data = 0x00aa;  // 0xaa00;
    dc.SetRegister(address, data);
    EXPECT_EQ(dc.GetRegister(address), data);
  }
}

struct HoldingRegisterControllerFixture : public ::testing::Test {
  std::array<uint16_t, kRegisters> registers{};
  Modbus::RegisterDataStore dc{registers.data(), registers.size()};
  Modbus::HoldingRegisterController<typeof(dc)> cc{&dc};
};

TEST_F(HoldingRegisterControllerFixture, AddressValid) {
  EXPECT_TRUE(cc.ReadLocationValid(0, kRegisters));
  EXPECT_TRUE(!cc.ReadLocationValid(1, kRegisters));
  EXPECT_TRUE(!cc.ReadLocationValid(0, kRegisters + 1));
}

TEST_F(HoldingRegisterControllerFixture, RegisterReadWrite) {
  const uint16_t data = 0x00aa;  // 0xaa00;
  for (std::size_t address = 0; address < kRegisters; address++) {
    cc.WriteRegister(address, data);
    const uint16_t reading = cc.ReadRegister(address);
    EXPECT_EQ(data, reading);
    EXPECT_TRUE(cc.ReadLocationValid(address, 1));
  }
}
TEST_F(HoldingRegisterControllerFixture, RegisterReadWriteAddress) {
  for (std::uint16_t address = 0; address < kRegisters; address++) {
    cc.WriteRegister(address, address);
    const uint16_t reading = cc.ReadRegister(address);
    EXPECT_EQ(address, reading);
  }
  for (std::uint16_t address = 0; address < kRegisters; address++) {
    const uint16_t reading = cc.ReadRegister(address);
    EXPECT_EQ(address, reading);
  }
}

struct ValidateHoldingRegisterFramesFixture : public ::testing::Test {
  std::array<uint16_t, kRegisters> registers{};
  Modbus::RegisterDataStore dc{registers.data(), registers.size()};
  Modbus::HoldingRegisterController<typeof(dc)> cc{&dc};
  std::array<uint8_t, 64> buffer;
  ArrayView<uint8_t> frame_data{buffer.size(), buffer.data()};

  const uint8_t kSlaveAddress = 0x11;
  const uint16_t kAddressStart = 0x0a;
  Modbus::Frame frame{kSlaveAddress, Modbus::Function::kNone, frame_data.size(),
                      frame_data};
  void SetUp(void) {}
  void TearDown(void) {}
  ValidateHoldingRegisterFramesFixture(void) {}
};

TEST_F(ValidateHoldingRegisterFramesFixture,
       ValidateReadMultipleHoldingRegistersCommand) {
  const uint16_t register_count = 12;
  Modbus::ReadMultipleHoldingRegistersCommand::FillFrame(
      kAddressStart, register_count, &frame);
  EXPECT_TRUE(Modbus::Exception::kAck == cc.ValidateFrame(frame));
}
TEST_F(ValidateHoldingRegisterFramesFixture,
       ValidateWriteSingleHoldingRegisterFrame) {
  Modbus::WriteSingleHoldingRegisterCommand::FillFrame(kAddressStart, 0xff,
                                                       &frame);
  const Modbus::Exception exception = cc.ValidateFrame(frame);
  EXPECT_TRUE(Modbus::Exception::kAck == exception);
}
TEST_F(ValidateHoldingRegisterFramesFixture,
       ValidateWriteMultipleHoldingRegistersFrame) {
  const uint16_t register_count = 16;
  constexpr std::size_t kRegisterBytes = register_count * sizeof(uint16_t);
  std::array<uint8_t, kRegisterBytes> write_data_buffer{};
  ArrayView<uint8_t> write_array_view{write_data_buffer.size(),
                                      write_data_buffer.data()};
  cc.ReadRegisters(kAddressStart, register_count, &write_array_view);

  constexpr std::array<uint16_t, register_count> register_values_buffer{};
  const ArrayView<const uint16_t> register_values{
      register_values_buffer.size(), register_values_buffer.data()};

  Modbus::WriteMultipleHoldingRegistersCommand::FillFrame(
      kAddressStart, register_count, register_values, &frame);
  const Modbus::Exception exception = cc.ValidateFrame(frame);
  EXPECT_TRUE(Modbus::Exception::kAck == exception);
}

TEST_F(ValidateHoldingRegisterFramesFixture,
       ReadFrameReadFrameReadMultipleHoldingRegisters) {
  Modbus::Response response;
  const uint16_t register_count = 12;
  Modbus::ReadMultipleHoldingRegistersCommand::FillFrame(
      kAddressStart, register_count, &frame);
  EXPECT_TRUE(Modbus::Exception::kAck == cc.ValidateFrame(frame));
  cc.ReadFrame(frame, &response);
  EXPECT_EQ(register_count * sizeof(uint16_t),
            Modbus::ReadMultipleHoldingRegistersCommand::ReadResponseByteCount(
                response.GetArrayView()));
}

TEST_F(ValidateHoldingRegisterFramesFixture,
       ReadFrameWriteSingleHoldingRegister) {
  const uint16_t kValue = 0xdead;
  Modbus::Response response;
  Modbus::WriteSingleHoldingRegisterCommand::FillFrame(kAddressStart, kValue,
                                                       &frame);
  const Modbus::Exception exception = cc.ValidateFrame(frame);
  EXPECT_TRUE(Modbus::Exception::kAck == exception);
  cc.ReadFrame(frame, &response);
  EXPECT_EQ(kAddressStart,
            Modbus::WriteSingleHoldingRegisterCommand::ReadResponseAddressStart(
                response.GetArrayView()));
  EXPECT_EQ(kValue,
            Modbus::WriteSingleHoldingRegisterCommand::ReadResponseSetting(
                response.GetArrayView()));
}

TEST_F(ValidateHoldingRegisterFramesFixture,
       ReadFrameWriteMultipleHoldingRegisters) {
  Modbus::Response response;
  const uint16_t register_count = 16;
  std::array<uint16_t, register_count> write_data_buffer{};
  const ArrayView<const uint16_t> write_array_view{write_data_buffer.size(),
                                                   write_data_buffer.data()};

  Modbus::WriteMultipleHoldingRegistersCommand::FillFrame(
      kAddressStart, register_count, write_array_view, &frame);
  const Modbus::Exception exception = cc.ValidateFrame(frame);
  EXPECT_TRUE(Modbus::Exception::kAck == exception);
  cc.ReadFrame(frame, &response);
  EXPECT_EQ(
      kAddressStart,
      Modbus::WriteMultipleHoldingRegistersCommand::ReadResponseAddressStart(
          response.GetArrayView()));
  EXPECT_EQ(
      register_count,
      Modbus::WriteMultipleHoldingRegistersCommand::ReadResponseRegisterCount(
          response.GetArrayView()));
}
}  // namespace ModbusTests
