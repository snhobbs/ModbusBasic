/* Copyright (C) 2020 Electrooptical Innovations
 * ----------------------------------------------------------------------
 * Project:      Modbus
 * Title:        test_ConnectedDevice.cpp
 * Description:  
 *
 * $Date:        13. May 2020
 * $Revision:    V.1.0.1
 * ----------------------------------------------------------------------
 */

#include "ConnectedSystem.h"
#include "Crc.h"
#include <Modbus/Modbus.h>
#include <Modbus/ModbusRtu/ModbusRtuSlave.h>
//#include <Modbus/ModbusRtu/ModbusRtuMaster.h>
#include <ArrayView/ArrayView.h>
#include <gtest/gtest.h>
#include <array>
#include <iostream>
#include <vector>
#include <cassert>
#include <cstdint>

#if 0
namespace ConnectedDevice {

struct ConnectedDeviceFixture {
  ConnectedSystem cs{};
};

TEST_FIXTURE(ConnectedDeviceFixture, startup) { cs.Setup(); }
}  // namespace ConnectedDevice
#endif
