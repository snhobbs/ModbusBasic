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
#include "BitField.h"
#include <ArrayView/ArrayView.h>
#include <Utilities/CommonTypes.h>
#include <Utilities/Crc.h>
#include <Utilities/TypeConversion.h>
#include <array>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <limits>

namespace Modbus {
class DataStore {
 protected:
  static const std::size_t kByteSize = 8;
  static const std::size_t kAddressStart = 0x0;

 public:
  static constexpr std::size_t GetAddressStart(void) { return kAddressStart; }
};


template<std::size_t kElements>
class BitFieldDataStore : public DataStore {
  BitField<kElements> bit_field_{}; 

 public:
  constexpr BitFieldDataStore() {}
  std::size_t size() const { return bit_field_.size(); } 
  bool IsAddressValid(std::size_t address) const {
    return address + 1 <= GetAddressStart() + bit_field_.size();
  }
  bool ReadLocationValid(std::size_t address, std::size_t count) const {
    return IsAddressValid(address) && IsAddressValid(address + count -1);
  }
  bool ReadElement(uint16_t address) const {
    return bit_field_.ReadElement(address); 
  }

  void WriteElement(uint16_t address, bool state) {
    return bit_field_.WriteElement(address, state);
  }
  void ReadElementsToBytes(const uint16_t starting_address,
                           const uint16_t element_count,
                           ArrayView<uint8_t> *response_data) const {
    return bit_field_.ReadElementsToBytes(starting_address, element_count, response_data);
  }
  void WriteMultipleElementsFromBytes(const uint16_t starting_address,
                                      const uint16_t element_count,
                                      const ArrayView<const uint8_t> data) {
    return bit_field_.WriteMultipleElementsFromBytes(starting_address, element_count, data);  
  }
};

template <std::size_t kSize, typename T = uint16_t>
class RegisterDataStore : public DataStore {
  std::array<T, kSize> data_store_{};
  static std::size_t GetIndex(T address) { return address - GetAddressStart(); }

 public:
  RegisterDataStore() {}
  static constexpr bool ReadLocationValid(std::size_t address, std::size_t count) {
    return IsAddressValid(address) && IsAddressValid(address + count -1);
  }
  static constexpr bool WriteLocationValid(std::size_t address, std::size_t count) {
    return ReadLocationValid(address, count);
  }
  static constexpr bool IsAddressValid(std::size_t address) {
    return address + 1 <= GetAddressStart() + GetSize();
  }
  static std::size_t size(void) {return GetSize();}
  static constexpr std::size_t GetSize(void) { return kSize; }
  static constexpr std::size_t GetRegisterByteSize(void) { return sizeof(T); }
  T GetRegister(std::size_t address) const {
    return data_store_[GetIndex(address)];
  }

  void GetRegisters(const std::size_t address, const std::size_t register_count, ArrayView<uint8_t>* data_view) const {
    const std::size_t num_data_bytes = register_count * sizeof(uint16_t);
    std::size_t register_index = address;
    for (std::size_t byte_number = 0; byte_number < num_data_bytes;) {
      const uint16_t state = GetRegister(register_index++);
      data_view->operator[](byte_number++) = Utilities::GetByte(state, 1);
      data_view->operator[](byte_number++) = Utilities::GetByte(state, 0);
    }
  }

  void SetRegister(std::size_t address, T value) {
    data_store_[GetIndex(address)] = value;
  }

  void SetRegisters(std::size_t address, std::size_t register_count, const ArrayView<const uint8_t>& data_view) {
    for (std::size_t index = 0; index < register_count; index++) {
      const std::size_t byte_index = index*sizeof(uint16_t);
      const ArrayView<const uint8_t> uint16_data_view{sizeof(uint16_t), &data_view[byte_index]};
      const uint16_t value = Utilities::Make_MSB_uint16_tFromU8Array(uint16_data_view);
      SetRegister(address + index, value);
    }
  }
};
}  //  namespace Modbus

#endif  // MODBUS_DATASTORE_H_
