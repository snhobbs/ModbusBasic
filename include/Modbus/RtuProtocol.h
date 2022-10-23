#pragma once
#include <cstddef>

#include "Modbus/Modbus.h"
namespace Modbus {
/*
 * Packet types are in Modbus.h, return the number of bytes necessary
 * before the command parser is guaranteed to be able to determine
 * the remaining number of bytes
 *
 * Options:
 *  + Return packet object -> then access common features
 *  + Have a function that addresses everything ...
 */

/*
 *  Needs to check packet for number of data bytes remaining
 *  Could have function pointers or interfaces
 */
inline int32_t CalculateBytesRemaining(const Frame& frame) {}
}  // namespace Modbus
