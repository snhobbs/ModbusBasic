/* Copyright (C) 2020 Electrooptical Innovations
 * ----------------------------------------------------------------------
 * Project:      Modbus
 * Title:        test_Coil.cpp
 * Description:  Test coil controller
 *               (coils & discrete inputs)
 *
 * $Date:        13. May 2020
 * $Revision:    V.1.0.1
 * ----------------------------------------------------------------------
 */

#include "Crc.h"
#include <ArrayView/ArrayView.h>
#include <Modbus/Modbus.h>
#include <Modbus/BitControl.h>
#include <Modbus/DataStore.h>
#include <Utilities/TypeConversion.h>
#include <gtest/gtest.h>
#include <iostream>
#include <vector>
#include <array>
#include <cassert>
#include <cstdint>

namespace ModbusTests {

struct CoilDataFixture : public ::testing::Test {
  static const constexpr std::size_t kCoilCount = 69;
  Modbus::BitFieldDataStore<kCoilCount> dc{};
};
TEST_F(CoilDataFixture, BitFieldRead) {
    for (std::size_t address = dc.GetAddressStart(); address < dc.GetAddressStart() + dc.size(); address++) {
    dc.WriteElement(address, true);
    EXPECT_TRUE(dc.ReadElement(address) == true);
    dc.WriteElement(address, false);
    EXPECT_TRUE(dc.ReadElement(address) == false);
    dc.WriteElement(address, true);
    EXPECT_TRUE(dc.ReadElement(address) == true);
  }
  for (std::size_t index = 0; index < dc.size(); index++) {
    std::size_t address = dc.GetAddressStart() + index;
    EXPECT_TRUE(dc.ReadElement(address) == true);
  }
  EXPECT_TRUE(dc.ReadElement(kCoilCount - 1) == true);
}

#if 0
TEST_F(CoilDataFixture, BitFieldWriteSingleBit) {
  for (std::size_t index = 0; index < dc.size(); index++) {
    std::size_t address = dc.GetAddressStart() + index;
    dc.WriteElement(address, true);
    EXPECT_TRUE(dc.ReadElement(address) == true);

    for (std::size_t sub_address = dc.GetAddressStart(); sub_address < dc.GetAddressStart() + dc.size(); sub_address++) {
      if (sub_address == address) {
        EXPECT_TRUE(dc.ReadElement(sub_address) == true);
      } else {
        EXPECT_TRUE(dc.ReadElement(sub_address) == false);
      }
    }
    dc.WriteElement(address, false);
    EXPECT_TRUE(dc.ReadElement(address) == false);
  }
}
#endif

struct CoilControllerFixture : public CoilDataFixture {
  Modbus::CoilController<typeof(dc)> cc;
};

TEST_F(CoilControllerFixture, CoilControllerReadWrite) {
  for (std::size_t address = 0; address < 0xffff; address++) {
    if (cc.IsAddressValid(address)) {
      cc.WriteElement(address, true);
      EXPECT_TRUE(cc.ReadCoil(address) == Modbus::CoilState::kOn);
      cc.WriteElement(address, false);
      EXPECT_TRUE(cc.ReadCoil(address) == Modbus::CoilState::kOff);
      cc.WriteElement(address, true);
      EXPECT_TRUE(cc.ReadCoil(address) == Modbus::CoilState::kOn);
    }
  }
}

TEST(ModbusCoils, WriteSingleCoilResponse) {
  Modbus::Response response;
  const Modbus::CoilState state = Modbus::CoilState::kOn;
  const uint16_t address = 0x34;
  const uint8_t slave_address = 0x56;
  Modbus::WriteSingleCoilCommand::FillResponseHeader(slave_address, address,
                                                     state, &response);
  EXPECT_EQ(slave_address,
              response[Modbus::Command::ResponsePacket::kSlaveAddress]);
  EXPECT_EQ(
      static_cast<int>(Modbus::Function::kWriteSingleCoil),
      static_cast<int>(response[Modbus::Command::ResponsePacket::kFunction]));

  const uint16_t response_coil_address = Modbus::WriteSingleCoilCommand::ReadResponseDataAddress(response);

  EXPECT_EQ(address, response_coil_address);
}

TEST(ModbusCoils, WriteMultipleCoilsResponse) {
  Modbus::Response response;
  const uint16_t coil_count = 0xccaa;
  const uint16_t address = 0x34;
  const uint8_t slave_address = 0x56;
  Modbus::WriteMultipleCoilsCommand::FillResponseHeader(slave_address, address,
                                                        coil_count, &response);
  EXPECT_EQ(slave_address,
              response[Modbus::Command::ResponsePacket::kSlaveAddress]);
  EXPECT_EQ(
      static_cast<int>(Modbus::Function::kWriteMultipleCoils),
      static_cast<int>(response[Modbus::Command::ResponsePacket::kFunction]));
  const uint16_t response_coil_address = Modbus::WriteMultipleCoilsCommand::ReadResponseDataAddressStart(response);
  EXPECT_EQ(address, response_coil_address);
  const uint16_t response_coil_count = Modbus::WriteMultipleCoilsCommand::ReadResponseCoilCount(response);
  EXPECT_EQ(coil_count, response_coil_count);
}

struct CoilControllerArbitraryDataFixture : public CoilControllerFixture {
  static const std::size_t kBytes = 4;
  static const constexpr std::size_t coil_count = (8 * kBytes) - 3;
  static const constexpr std::array<bool, coil_count> coil_settings{
      false, true, false, true,  false, true,  false, false, false, true,
      false, true, false, true,  false, false, false, true,  false, true,
      false, true, false, false, false, true,  false, true,  false};
  void ApplyCoilSettings(const uint16_t starting_address) {
    std::size_t address = starting_address;
    for (bool val : coil_settings) {
      if (val)
        cc.WriteElement(address++, true);
      else
        cc.WriteElement(address++, false);
    }
  }
};

#if 0
TEST_F(CoilControllerArbitraryDataFixture, CoilControllerReadWriteMulti) {
  const std::size_t starting_address = 0x00;
  ApplyCoilSettings(starting_address);
  std::size_t index = 0;
  while (cc.IsAddressValid((starting_address + index))) {
    if (index < coil_count) {
      EXPECT_EQ(coil_settings[index], cc.ReadCoil(starting_address + index) ==
                                            Modbus::CoilState::kOn);
    } else {
      EXPECT_TRUE(cc.ReadCoil(starting_address + index) == Modbus::CoilState::kOff);
    }
    index++;
  }
}
#endif
#if 0
//  Same format as
TEST_F(CoilControllerArbitraryDataFixture,
             CoilControllerReadIntoResponse) {
  Modbus::Response response;
  const std::size_t starting_address = 0x00;
  ApplyCoilSettings(starting_address);

  auto response_data = ArrayView<uint8_t>{response.size() - Modbus::ReadBitsCommand::kValueStart + 1, &response.data()[Modbus::ReadBitsCommand::kValueStart + 1]};
  cc.ReadCoilsIntoResponse(starting_address, coil_count, &response_data);
  Modbus::ReadBitsCommand::FillResponseHeader(coil_count, &response);
  CoilControllerFixture ccf;
  const std::size_t kReadCoilsByteCountPosition = 2;
  const std::size_t kReadCoilsResponseStart = kReadCoilsByteCountPosition + 1;
  const uint8_t byte_count = response[kReadCoilsByteCountPosition];
  ccf.cc.WriteMultipleCoils(
      starting_address, coil_count, byte_count,
      ArrayView{response.GetLength() - kReadCoilsResponseStart + 1,
                &response.data()[kReadCoilsResponseStart]});
  std::size_t index = 0;
  while (cc.IsAddressValid(static_cast<uint16_t>(starting_address + index))) {
    const auto write_value = ccf.cc.ReadCoil(starting_address + index);
    const auto read_value = cc.ReadCoil(starting_address + index);
    EXPECT_TRUE(write_value == read_value);
    index++;
  }
}
#endif
// Test Validate frame

struct ValidateFramesFixture : public CoilControllerFixture {
  std::array<uint8_t, 64> buffer;
  ArrayView<uint8_t> frame_data{buffer.size(), buffer.data()};
  const uint16_t kAddressStart = 0x00;
  Modbus::Frame frame{frame_data};
  ValidateFramesFixture() {
    // frame_data[Modbus::CoilCommand::kDataAddressStart] =
    // Utilities::GetByte(kAddressStart, 0);
    // frame_data[Modbus::CoilCommand::kDataAddressStart + 1] =
    // Utilities::GetByte(kAddressStart, 1);
  }
};
TEST_F(ValidateFramesFixture, ValidateReadCoilsFrame) {
  const uint16_t coil_count = 12;
  frame.function = Modbus::Function::kReadCoils;
  Modbus::ReadBitsCommand::FillFrame(kAddressStart, coil_count, &frame);
  EXPECT_TRUE(Modbus::Exception::kAck == cc.ValidateFrame(frame));
}

TEST_F(ValidateFramesFixture, ValidateWriteSingleCoilFrame) {
    frame.function = Modbus::Function::kWriteSingleCoil;
  Modbus::WriteSingleCoilCommand::FillFrame(kAddressStart,
                                            Modbus::CoilState::kOff, &frame);
  const Modbus::Exception exception = cc.ValidateFrame(frame);
  EXPECT_TRUE(Modbus::Exception::kAck == exception);
}
TEST_F(ValidateFramesFixture, ValidateWriteMultipleCoilsFrame) {
  const uint16_t coil_count = 16;
  std::array<uint8_t, 64> data_buffer{};
  ArrayView<uint8_t> array_view{Utilities::CalcNumberOfBytesToFitBits(coil_count),
                                data_buffer.data()};

  cc.ReadBits(kAddressStart, coil_count, &array_view);
  frame.function = Modbus::Function::kWriteMultipleCoils;
  Modbus::WriteMultipleCoilsCommand::FillFrame(
      kAddressStart, coil_count,
      ArrayView<const uint8_t>{array_view.size(), array_view.data()}, &frame);
  const Modbus::Exception exception = cc.ValidateFrame(frame);
  EXPECT_TRUE(Modbus::Exception::kAck == exception);
}

// FIXME Validate each type of busted frame

TEST_F(ValidateFramesFixture, ReadFrameReadCoils) {
  Modbus::Response response;
  const uint16_t coil_count = 12;
  frame.function = Modbus::Function::kReadCoils;
  Modbus::ReadBitsCommand::FillFrame(kAddressStart, coil_count, &frame);
  EXPECT_TRUE(Modbus::Exception::kAck == cc.ValidateFrame(frame));
  cc.ReadFrame(frame, &response);
}

TEST_F(ValidateFramesFixture, ReadFrameSingleCoil) {
  Modbus::Response response;
    frame.function = Modbus::Function::kWriteSingleCoil;
  Modbus::WriteSingleCoilCommand::FillFrame(kAddressStart,
                                            Modbus::CoilState::kOff, &frame);
  const Modbus::Exception exception = cc.ValidateFrame(frame);
  EXPECT_TRUE(Modbus::Exception::kAck == exception);
  cc.ReadFrame(frame, &response);
}
TEST_F(ValidateFramesFixture, ReadFrameWriteMultipleCoils) {
  Modbus::Response response;
  const uint16_t coil_count = 16;
  std::array<uint8_t, 64> data_buffer{};
  ArrayView<uint8_t> array_view{Utilities::CalcNumberOfBytesToFitBits(coil_count),
                                data_buffer.data()};

  cc.ReadBits(kAddressStart, coil_count, &array_view);
  frame.function = Modbus::Function::kWriteMultipleCoils;
  Modbus::WriteMultipleCoilsCommand::FillFrame(
      kAddressStart, coil_count,
      ArrayView<const uint8_t>{array_view.size(), array_view.data()}, &frame);
  const Modbus::Exception exception = cc.ValidateFrame(frame);
  EXPECT_TRUE(Modbus::Exception::kAck == exception);
  cc.ReadFrame(frame, &response);
}
// FIXME Add read frame test
}  // namespace ModbusTests
