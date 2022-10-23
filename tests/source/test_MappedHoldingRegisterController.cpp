/* Copyright (C) 2020 Electrooptical Innovations
 * ----------------------------------------------------------------------
 * Project:      Modbus
 * Title:        test_MappedHoldingRegisterController.cpp
 * Description:
 *
 * $Date:        13. May 2020
 * $Revision:    V.1.0.1
 * ----------------------------------------------------------------------
 */

#include <Modbus/MappedRegisterDataStore.h>
#include <Modbus/RegisterControl.h>
#include <gtest/gtest.h>

#include <array>
#include <cstdio>
#include <iostream>
#include <vector>

#include "TestHoldingRegisterMappedDataStore.h"

using HoldingRegisterController = Modbus::HoldingRegisterController<
    Modbus::MappedRegisterDataStore<ModbusBasic_holding_register::Wrapper>>;
using HoldingRegisterWrapper = ModbusBasic_holding_register::Wrapper;

TEST(HoldingRegister, GetMap) {
  ModbusBasic_holding_register::holding_register data_store;
  HoldingRegisterWrapper map{&data_store};
  Modbus::MappedRegisterDataStore<HoldingRegisterWrapper>
      holding_register_data_store_{&map};
}

class MappedHoldingRegisterControllerFixture : public ::testing::Test {
 public:
  ModbusBasic_holding_register::holding_register data_store;
  ModbusBasic_holding_register::Wrapper map{&data_store};
  Modbus::MappedRegisterDataStore<HoldingRegisterWrapper>
      holding_register_data_store_{&map};
  HoldingRegisterController controller{&holding_register_data_store_};
};

TEST_F(MappedHoldingRegisterControllerFixture,
       HoldingRegisterController_WriteLocationValid_ReturnsValid) {
  /*
   * Pass in a valid location, check it returns valid
   * */
  for (size_t i = 0; i < map.end_points_.size(); i++) {
    const auto length = map.end_points_[i] - map.offsets_[i] + 1;
    const auto address = map.offsets_[i];
    const bool valid = controller.WriteLocationValid(address, length);
    //  printf("\n%d: %d %d\n", valid, address, length);
    EXPECT_TRUE(valid);
  }
}

#if 0
  bool WriteLocationValid(std::size_t address, std::size_t count) {
  bool ReadLocationValid(std::size_t address, std::size_t count) {
  bool IsAddressValid(std::size_t address) {
  std::size_t size(void) {return GetSize();}
  std::size_t GetSize(void) { return MapObject::size(); }
  std::size_t GetRegisterByteSize(void) { return sizeof(uint16_t); }
  uint16_t GetRegister(std::size_t address) const {}
  void GetRegisters(std::size_t address, std::size_t register_count, ArrayView<uint8_t>* data_view) const {}
  void SetRegister(std::size_t address, uint16_t value) {}
  void SetRegisters(std::size_t address, std::size_t register_count, const ArrayView<const uint8_t>& data_view) {}
#endif
