/* Copyright (C) 2020 Electrooptical Innovations
 * ----------------------------------------------------------------------
 * Project:      Modbus
 * Title:        ModbusRtu.h
 * Description:  Includes the headers needed for modbus slaves and masters
 *
 * $Date:        13. May 2020
 * $Revision:    V.1.0.1
 * ----------------------------------------------------------------------
 */

#pragma once
#ifndef MODBUS_MODBUSRTU_H_
#define MODBUS_MODBUSRTU_H_

#include <ArrayView/ArrayView.h>
//#include <Modbus/BitFieldDataController.h>
#include <Modbus/DeviceIdentifier.h>
#include <Modbus/Modbus.h>
//#include <Modbus/RegisterDataController.h>
#include <Modbus/DataStore.h>
#include <Modbus/DataCommand.h>
#include <Modbus/BitControl.h>
#include <Modbus/DataTypeController.h>
#include <Modbus/RegisterControl.h>
#include <Modbus/ModbusRtu/ModbusRtuProtocol.h>
#include <RingBuffer/RingBuffer.h>
#include <Utilities/CommonTypes.h>
#include <Utilities/Crc.h>
#include <Utilities/TypeConversion.h>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <limits>
namespace Modbus {
}  //  namespace Modbus
/*
 * Needs mechanism to check that the minimum amount of time between frames has
 * been satisfied -> could use a no character symbol in a uart wrapper... need
 * to assign registers, coils etc.
 *
 * Add response reading to the command classes
 *
 * */

#endif  //  MODBUS_MODBUSRTU_H_
