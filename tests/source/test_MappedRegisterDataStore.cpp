/* Copyright (C) 2020 Electrooptical Innovations
 * ----------------------------------------------------------------------
 * Project:      Modbus
 * Title:        test_MappedRegisterDataStore.cpp
 * Description:
 *
 * $Date:        13. May 2020
 * $Revision:    V.1.0.1
 * ----------------------------------------------------------------------
 */

#include <Modbus/MappedRegisterDataStore.h>
#include <gtest/gtest.h>

#include <array>
#include <iostream>
#include <vector>

#include "TestHoldingRegisterMappedDataStore.h"
namespace TestsMappedRegisterDataStore {
/*
using HoldingRegisterController =
    Modbus::HoldingRegisterController<Modbus::MappedRegisterDataStore<ModbusBasic_holding_register::Wrapper>>;
*/
/*using InputRegisterController =
  Modbus::InputRegisterController<Modbus::MappedRegisterDataStore<ModbusBasic_input_register::Wrapper>>;
using SlaveBase =
 Modbus::ProtocolRtuSlave<HoldingRegisterController,
                           InputRegisterController>;
*/

using HoldingRegisters = ModbusBasic_holding_register::holding_register;
using HoldingRegistersWrapper = ModbusBasic_holding_register::Wrapper;

TEST(check_location_valid, vaid_returns_valid) {
  const size_t size = 2;
  using namespace Modbus;
  std::array<size_t, 64> addresses{};
  std::array<size_t, 64> end_points{};
  //  length of addresses
  for (size_t i = 0; i < addresses.size(); i++) {
    addresses[i] = i + 6;
    end_points[i] = addresses[i] + size - 1;
  }

  EXPECT_TRUE(check_location_valid(addresses[0], size,
                                   {addresses.cbegin(), addresses.size()},
                                   {end_points.cbegin(), end_points.size()}));
}

TEST(get_matching_index, valid_return) {
  using namespace Modbus;
  std::array<size_t, 64> addresses{0, 0, 1, 2, 3, 4, 5, 1};
  const size_t value = 0xfeed;
  const size_t index = 52;
  addresses[index] = value;
  EXPECT_EQ(index, get_matching_index<size_t>(
                       value, {addresses.cbegin(), addresses.size()}));
}

TEST(DataMapTest, Serialization) {}

TEST(DataMapTest, GettersAndSetters) {
  HoldingRegisters data_store;
  HoldingRegistersWrapper map{&data_store};
  std::array<uint8_t, 64> buff{'a', 'b', 'c', 'd', 'e', 'f',
                               'g', 'h', 'j', 'k', 'l'};
  ArrayView<uint8_t> data_view{buff.size(), buff.data()};

  const auto str = map.get_str();
  map.set_str(buff.data(), str.size());
  EXPECT_EQ(str.size(), sizeof(HoldingRegisters::str));
  for (std::size_t i = 0; i < str.size(); i++) {
    EXPECT_EQ(str[i], buff[i]);
  }
  map.set_int32(1026);
  EXPECT_EQ(map.get_int32(), 1026);

  map.set_int645(1000026);
  EXPECT_EQ(map.get_int645(), 1000026);

  EXPECT_EQ(map.get_int643(), 0);
}

/*
 * Set field, read, set a new value, write original value array, check equal to
 * original setting
 */
TEST(DataMapTest, Serialize) {
  HoldingRegisters data_store;
  HoldingRegistersWrapper map{&data_store};
  std::array<uint8_t, 64> data{0};
  ArrayView<uint8_t> data_view{data.size(), data.data()};
  EXPECT_EQ(map.get_int645(), 0);

  const uint64_t setting = 999999999;
  map.set_int645(setting);
  EXPECT_EQ(map.get_int645(), setting);

  const uint64_t new_setting = 44444444444;
  map.set_int645(new_setting);
  EXPECT_EQ(map.get_int645(), new_setting);

  //  map.SetField(HoldingRegisters::MemoryMapEntryIdentifier::int645,
  //  ArrayView<uint8_t>{sizeof(uint64_t), data_view.data()});
  //  EXPECT_EQ(map.get_int645(), setting);
}

}  // namespace TestsMappedRegisterDataStore
