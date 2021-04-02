#include <cstring>
#include <cassert>
#include <cstdio>
#include <gtest/gtest.h>
#include <stdint.h>
#include <array>
#include <iostream>
#include <vector>
#include <Modbus/flat_buffers_register_wrapper.h>
#include <Modbus/Modbus.h>
#include <Modbus/BitControl.h>
#include <Modbus/DataStore.h>
#include <Modbus/ModbusRtu/ModbusRtuSlave.h>
#include <Modbus/RegisterControl.h>
#include <Modbus/MappedRegisterDataStore.h>
#include <Modbus/DataStore.h>
#include "input_registers_generated.h"
#include "holding_registers_generated.h"

namespace FlatBuffersAdapterTests {
inline const constexpr std::size_t kCoilCount = 1;
using CoilController =
    Modbus::CoilController<Modbus::BitFieldDataStore<kCoilCount>>;
using DiscreteInputController =
    Modbus::DiscreteInputController<Modbus::BitFieldDataStore<kCoilCount>>;

using HoldingRegisters = Modbus::HoldingRegisters::Registers;
using HoldingRegisterMemoryMap = Modbus::FlatBufferRegisterStore<HoldingRegisters>;
using HoldingRegisterController =
    Modbus::HoldingRegisterController<Modbus::MappedRegisterDataStore<HoldingRegisterMemoryMap>>;

using InputRegisters = Modbus::InputRegisters::Registers;
using InputRegisterMemoryMap = Modbus::FlatBufferRegisterStore<InputRegisters>;
using InputRegisterController =
    Modbus::InputRegisterController<Modbus::MappedRegisterDataStore<InputRegisterMemoryMap>>;
using SlaveBase =
    Modbus::ProtocolRtuSlave<CoilController, HoldingRegisterController,
                             DiscreteInputController, InputRegisterController>;



TEST(HoldingRegisters, mutate) {
  HoldingRegisters holding_registers{};
  holding_registers.mutate_u16(12);
  ASSERT_EQ(holding_registers.u16(), 12);
}

TEST(InputRegisters, mutate) {
  InputRegisters input_registers{};
  input_registers.mutate_u16(12);
  ASSERT_EQ(input_registers.u16(), 12);
} 

TEST(HoldingRegisterMemoryMap, construct) {
  HoldingRegisters holding_registers{};
  HoldingRegisterMemoryMap holding_register_data_map{&holding_registers};
}

TEST(InputRegisterMemoryMap, construct) {
  InputRegisters input_registers{};
  InputRegisterMemoryMap input_register_data_map{&input_registers};
}

TEST(MappedRegisterDataStore_HoldingRegisterMemoryMap, construct) {
  HoldingRegisters holding_registers{};
  HoldingRegisterMemoryMap holding_register_data_map{&holding_registers};
  Modbus::MappedRegisterDataStore<HoldingRegisterMemoryMap> holding_register_data_store{&holding_register_data_map};
}

TEST(MappedRegisterDataStore_InputRegisterMemoryMap, construct) {
  InputRegisters input_registers{};
  InputRegisterMemoryMap input_register_data_map{&input_registers};
  Modbus::MappedRegisterDataStore<InputRegisterMemoryMap> input_register_data_store{&input_register_data_map};
}

struct InputRegisterFlatBuffersDataFixture : public ::testing::Test {
  InputRegisters input_registers{};
  InputRegisterMemoryMap input_register_data_map{&input_registers};
  Modbus::MappedRegisterDataStore<InputRegisterMemoryMap> dc{&input_register_data_map};
  Modbus::InputRegisterController<typeof(dc)> cc{&dc};

  void SetUp(void) {}
  void TearDown(void) {}
};

TEST_F(InputRegisterFlatBuffersDataFixture, construct) {}

struct HoldingRegisterFlatBuffersDataFixture : public ::testing::Test {
  HoldingRegisters holding_registers{};
  HoldingRegisterMemoryMap holding_register_data_map{&holding_registers};
  Modbus::MappedRegisterDataStore<HoldingRegisterMemoryMap> dc{&holding_register_data_map};
  Modbus::HoldingRegisterController<typeof(dc)> cc{&dc};
  void SetUp(void) {}
  void TearDown(void) {}
};

TEST_F(HoldingRegisterFlatBuffersDataFixture, construct) {}

TEST_F(HoldingRegisterFlatBuffersDataFixture, Read_first_field_inbound) {
  const auto offset = holding_register_data_map.get_offset(0);
  const auto length = holding_register_data_map.get_field_size(0);
  std::array<uint8_t, 64> write_data{1,2,3,5};
  std::array<uint8_t, 64> read_data{1,2,3,5};
  dc.SetRegisters(offset/2, length/2, write_data.data(), write_data.size());
  dc.GetRegisters(offset/2, length/2, read_data.data(), read_data.size());
}
}
