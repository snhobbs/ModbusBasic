/* Copyright (C) 2020 Electrooptical Innovations
 * ----------------------------------------------------------------------
 * Project:      Modbus
 * Title:        DataStore.h
 * Description:  Default data stores for bits and registers
 *
 * $Date:        13. May 2020
 * $Revision:    V.1.0.1
 * ----------------------------------------------------------------------
 */

#pragma once
#ifndef MODBUS_DATASTORE_H_
#define MODBUS_DATASTORE_H_
#include <cstdint>

namespace Modbus {
class DataStore {
 protected:
  static const std::size_t kByteSize = 8;
  static const size_t kAddressStart = 0x00;

 public:
  static constexpr std::size_t GetAddressStart(void) { return kAddressStart; }
};

}  // namespace Modbus

#endif  // MODBUS_DATASTORE_H_
