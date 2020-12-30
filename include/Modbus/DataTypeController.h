/* Copyright (C) 2020 Electrooptical Innovations
 * ----------------------------------------------------------------------
 * Project:      Modbus
 * Title:        DataTypeController.h
 * Description:  
 *
 * $Date:        13. May 2020
 * $Revision:    V.1.0.1
 * ----------------------------------------------------------------------
 */

#pragma once
namespace Modbus {
template <std::size_t kSize>
class DataTypeController {
 public:
  static const uint16_t kAddressStart = 0x0;
  static constexpr uint16_t GetAddressStart(void) { return kAddressStart; }
  static constexpr uint16_t GetAddressEnd(void) {
    return kAddressStart + kSize - 1;
  }
  static constexpr std::size_t GetSize(void) { return kSize; }
};

template <>
class DataTypeController<0> {
 public:
  static const uint16_t kAddressStart = 0x0;
  static constexpr uint16_t GetAddressStart(void) { return kAddressStart; }
  static constexpr uint16_t GetAddressEnd(void) { return kAddressStart; }
  static constexpr std::size_t GetSize(void) { return 0; }
};
}  //  namespace Modbus
