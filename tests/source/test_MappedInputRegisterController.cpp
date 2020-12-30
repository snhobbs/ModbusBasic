/* Copyright (C) 2020 Electrooptical Innovations
 * ----------------------------------------------------------------------
 * Project:      Modbus
 * Title:        test_MappedInputRegisterController.cpp
 * Description:  
 *
 * $Date:        13. May 2020
 * $Revision:    V.1.0.1
 * ----------------------------------------------------------------------
 */

#include "TestInputRegisterMappedDataStore.h"
#include <Modbus/RegisterControl.h>
#include <Modbus/MappedRegisterDataStore.h>
#include <gtest/gtest.h>
#include <iostream>
#include <vector>
#include <array>

TEST(InputRegister, GetMap) {
  Modbus::MappedRegisterDataStore<InputRegisters::DataMap> data_controller;
  InputRegisters::DataMap& map = data_controller.GetMapObject();
}
TEST(InputRegister, Instantiate) {
  Modbus::InputRegisterController<Modbus::MappedRegisterDataStore<InputRegisters::DataMap>> input_controller;
}
#if 0
  bool WriteLocationValid(std::size_t address, std::size_t count) {
  bool ReadLocationValid(std::size_t address, std::size_t count) {
  bool IsAddressValid(std::size_t address) {
  std::size_t size(void) {return GetSize();}
  std::size_t GetSize(void) { return MapObject::size(); }
  std::size_t GetRegisterByteSize(void) { return sizeof(uint16_t); }
  uint16_t GetRegister(std::size_t address) const {
  void GetRegisters(std::size_t address, std::size_t register_count, ArrayView<uint8_t>* data_view) const {
  void SetRegister(std::size_t address, uint16_t value) {
  void SetRegisters(std::size_t address, std::size_t register_count, const ArrayView<const uint8_t>& data_view) {
#endif
