#pragma once

#include <ArrayView/ArrayView.h>
#include <Utilities/CommonTypes.h>
#include <Utilities/Crc.h>
#include <Utilities/TypeConversion.h>
#include <limits>
#include <array>
#include <cassert>
#include <cmath>

#include "BitField.h"
#include "Modbus/DataStore/DataStore.h"

namespace Modbus {
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

}
