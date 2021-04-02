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

/*
 * Basic register data store has each register as an entry, all in range
 * accesses are legal
 * */
template <std::size_t kSize, typename T = uint16_t>
class RegisterDataStore : public DataStore {
  std::array<T, kSize> data_store_{};
  static std::size_t GetIndex(T address) { return address - GetAddressStart(); }

 public:
  RegisterDataStore() {}
  static const size_t number_of_fields = kSize;
  static std::size_t size(void) {return GetSize();}
  static constexpr std::size_t GetSize(void) { return kSize; }
  static constexpr std::size_t GetRegisterByteSize(void) { return sizeof(T); }

  static size_t get_field_size(const size_t index) {
    return sizeof(T);
  }

  static size_t get_offset(const size_t index) {
    return index*sizeof(T);
  }

  bool ReadLocationValid(std::size_t byte_address, std::size_t byte_count) const {
    return WriteLocationValid(byte_address, byte_count);
  }

  bool WriteLocationValid(std::size_t byte_address, std::size_t byte_count) const {
    const bool valid = byte_address < size()*sizeof(T) && byte_count <= size()*sizeof(T)-byte_address;
    return valid;
  }

  void read(const size_t byte_address, const size_t count, uint8_t* data) const {
    const size_t register_count = count/sizeof(uint16_t);
    const size_t register_address = byte_address/sizeof(uint16_t);
    for (std::size_t index = 0; index < register_count; index++) {
      const uint16_t state = data_store_[register_address+index];
      data[index*sizeof(uint16_t)] = 0xFF&(state >> 8);
      data[index*sizeof(uint16_t)+1] = 0xFF&(state);
    }
  }

  void write(const size_t byte_address, const size_t count, const uint8_t* data) {
    const size_t register_count = count/sizeof(uint16_t);
    const size_t register_address = byte_address/sizeof(uint16_t);
    for (std::size_t index = 0; index < register_count; index++) {
      const uint16_t state = static_cast<uint16_t>(data[index*sizeof(uint16_t)]) << 8 | data[index*sizeof(uint16_t)+1];
      data_store_[index+register_address] = state;
    }
  }
};
}  //  namespace Modbus

#endif  // MODBUS_DATASTORE_H_
