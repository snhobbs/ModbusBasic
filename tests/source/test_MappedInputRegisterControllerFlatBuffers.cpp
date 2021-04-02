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

#include "input_registers_generated.h"
#include <Modbus/flat_buffers_register_wrapper.h>
#include <Modbus/MappedRegisterDataStore.h>
#include <Modbus/RegisterControl.h>
#include <gtest/gtest.h>
#include <array>
#include <iostream>
#include <vector>

TEST(FlatBuffersRegisterInput, Constuct) {
  Modbus::FlatBufferRegisterStore<Modbus::InputRegisters::Registers> input_map_{};
  Modbus::MappedRegisterDataStore<typeof(input_map_)> input_register_data_store_{&input_map_};
}

#if 0
TEST(FlatBuffersRegister, SetCompleteField) {
  Modbus::HoldingRegisters::Registers holding_map_;
  Modbus::MappedRegisterDataStore<Modbus::HoldingRegisters::Registers> holding_register_data_store_{&holding_map_};
  holding_register_data_store_.SetField(1, );
}
#endif
