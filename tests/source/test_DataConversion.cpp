/* Copyright (C) 2020 Electrooptical Innovations
 * ----------------------------------------------------------------------
 * Project:      Modbus
 * Title:        test_DataConversion.cpp
 * Description:
 *
 * $Date:        13. May 2020
 * $Revision:    V.1.0.1
 * ----------------------------------------------------------------------
 */

#include <Modbus/Modbus.h>
#include <Modbus/ModbusRtu/ModbusRtuSlave.h>

#include "ConnectedSystem.h"
#include "Crc.h"
//#include <Modbus/ModbusRtu/ModbusRtuMaster.h>
#include <ArrayView/ArrayView.h>
#include <Utilities/TypeConversion.h>
#include <gtest/gtest.h>

#include <array>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <vector>

TEST(ModbusData, LSBmaker) {
  const uint16_t kData = 0xdead;
  const auto u8_array = Utilities::MakeLSBU8Array(kData);
  EXPECT_EQ(sizeof(kData), u8_array.size());
  EXPECT_EQ(kData, (u8_array[0] + (u8_array[1] << 8)));
  EXPECT_EQ(kData, Utilities::Make_LSB_uint32_tFromU8Array(u8_array.data(),
                                                           u8_array.size()));
  const ArrayView<const uint8_t> u8_view{u8_array.size(), u8_array.data()};
  EXPECT_EQ(kData, Utilities::Make_LSB_uint16_tFromU8Array(u8_view));
}
