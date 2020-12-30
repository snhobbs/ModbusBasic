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
  // static const constexpr std::size_t kLog2Of8 = std::log2(8);

  static const constexpr std::size_t kLog2Of8 = 3;
  static_assert(1 << kLog2Of8 == 8);
  // static const constexpr std::size_t kDataBytes = kSize / 8 + 1;
  static const constexpr std::size_t kByteCount =
        Utilities::CalcNumberOfBytesToFitBits(kElements);

  std::array<uint8_t, kByteCount> data_store_ __attribute__((aligned (sizeof(std::size_t))));

 public:
  static std::size_t size(void) {return GetSize();}
  static constexpr std::size_t GetSize(void) { return kElements; }
  static constexpr std::size_t GetDataArrayIndex(std::size_t address) {
    return address >> kLog2Of8;
  }
  static constexpr std::size_t GetDataByteIndex(std::size_t address) {
    return static_cast<std::size_t>(address & ((1 << kLog2Of8) - 1));
  }
  static constexpr uint8_t GetByteMask(std::size_t address) {
    return static_cast<uint8_t>(1 << GetDataByteIndex(address));
  }
  static constexpr bool IsAddressValid(std::size_t address) {
    return address + 1 <= GetAddressStart() + GetSize();
  }
  static constexpr bool ReadLocationValid(std::size_t address, std::size_t count) {
    return IsAddressValid(address) && IsAddressValid(address + count -1);
  }
  bool ReadElement(uint16_t address) const {
    return data_store_[GetDataArrayIndex(address)] & GetByteMask(address);
  }

  void WriteElement(uint16_t address, bool state) {
    if (state) {
      data_store_[GetDataArrayIndex(address)] |= (GetByteMask(address));
    } else {
      data_store_[GetDataArrayIndex(address)] &= ~(GetByteMask(address));
    }
  }
  void ReadElementsToBytes(const uint16_t starting_address,
                           const uint16_t element_count,
                           ArrayView<uint8_t> *response_data) const {
    const std::size_t num_data_bytes =
        Utilities::CalcNumberOfBytesToFitBits(element_count);
    std::size_t address = starting_address;
    for (std::size_t byte_number = 0; byte_number < num_data_bytes;
         byte_number++) {
      uint8_t byte_value = 0;
      for (std::size_t byte_index = 0; byte_index < kByteSize; byte_index++) {
        const bool state = ReadElement(address++);
        if (state) {
          byte_value |= static_cast<uint8_t>(1 << byte_index);
        }
      }
      response_data->operator[](byte_number) = byte_value;
    }
  }
  void WriteMultipleElementsFromBytes(const uint16_t starting_address,
                                      const uint16_t element_count,
                                      const ArrayView<const uint8_t> data) {
    const std::size_t num_data_bytes =
        Utilities::CalcNumberOfBytesToFitBits(element_count);
    assert(data.size() >= num_data_bytes);
    std::size_t address = starting_address;
    for (std::size_t byte_number = 0; byte_number < num_data_bytes;
         byte_number++) {
      for (std::size_t byte_index = 0; byte_index < kByteSize; byte_index++) {
        const bool flag_value = (data[byte_number] & (1 << byte_index)) != 0;
        WriteElement(address++, flag_value);
      }
    }
  }

  constexpr BitFieldDataStore() {}
};

template <std::size_t kSize, typename T = uint16_t>
class RegisterDataStore : public DataStore {
  std::array<T, kSize> data_store_{};
  static std::size_t GetIndex(T address) { return address - GetAddressStart(); }

 public:
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
  constexpr RegisterDataStore() {}
};
}  //  namespace Modbus

#endif  // MODBUS_DATASTORE_H_
