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

#include "TestHoldingRegisterMappedDataStore.h"
#include <Modbus/MappedRegisterDataStore.h>
#include <gtest/gtest.h>
#include <iostream>
#include <vector>
#include <array>
namespace TestsMappedRegisterDataStore {
TEST(DataMapTest, Serialization) {

}

TEST(DataMapTest, GettersAndSetters) {
  HoldingRegisters::MemoryMap data_store;
  HoldingRegisters::MemoryMapController map{&data_store};
  std::array<uint8_t, 64> buff{'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'j', 'k', 'l'};
  ArrayView<uint8_t> data_view{buff.size(), buff.data()};

  const auto str = map.get_str();
  map.set_str(buff.data(), str.size());
  EXPECT_EQ(str.size(), sizeof(HoldingRegisters::MemoryMap::str));
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
 * Set field, read, set a new value, write original value array, check equal to original setting
 */
TEST(DataMapTest, Serialize) {
  HoldingRegisters::MemoryMap data_store;
  HoldingRegisters::MemoryMapController map{&data_store};
  std::array<uint8_t, 64> data{0};
  ArrayView<uint8_t> data_view{data.size(), data.data()};
  EXPECT_EQ(map.get_int645(), 0);

  const uint64_t setting = 999999999;
  map.set_int645(setting);
  EXPECT_EQ(map.get_int645(), setting);

  const uint64_t new_setting = 44444444444;
  map.set_int645(new_setting);
  EXPECT_EQ(map.get_int645(), new_setting);

  //  map.SetField(HoldingRegisters::MemoryMapEntryIdentifier::int645, ArrayView<uint8_t>{sizeof(uint64_t), data_view.data()});
  //  EXPECT_EQ(map.get_int645(), setting);
}

}  // namespace TestsMappedRegisterDataStore
