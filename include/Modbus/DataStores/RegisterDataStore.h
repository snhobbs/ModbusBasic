#include <ArrayView/ArrayView.h>
#include <Utilities/CommonTypes.h>
#include <Utilities/Crc.h>
#include <Utilities/TypeConversion.h>
#include <limits>
#include <array>
#include <cassert>
#include <cmath>
#include <utility>  // pair

#include "Modbus/DataStores/DataStore.h"

namespace Modbus {
class RegisterDataStore : public DataStore {
  std::pair<uint16_t*, size_t> data_store_;
  static std::size_t GetIndex(uint16_t address) { return address - GetAddressStart(); }

 public:
  RegisterDataStore(uint16_t* data, const size_t length) : data_store_{data, length}{}
  bool ReadLocationValid(std::size_t address, std::size_t count) const {
    return IsAddressValid(address) && IsAddressValid(address + count -1);
  }
  bool WriteLocationValid(std::size_t address, std::size_t count) const {
    return ReadLocationValid(address, count);
  }
  bool IsAddressValid(std::size_t address) const {
    return address + 1 <= GetAddressStart() + GetSize();
  }
  std::size_t size(void) {return GetSize();}
  std::size_t GetSize(void) const { return data_store_.second; }
  static constexpr std::size_t GetRegisterByteSize(void) { return sizeof(uint16_t); }
  uint16_t GetRegister(std::size_t address) const {
    return data_store_.first[GetIndex(address)];
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

  void SetRegister(std::size_t address, uint16_t value) {
    data_store_.first[GetIndex(address)] = value;
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


