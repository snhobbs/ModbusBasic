#include <ArrayView/ArrayView.h>
#include <Utilities/CommonTypes.h>
#include <Utilities/Crc.h>
#include <Utilities/TypeConversion.h>

#include <array>
#include <cassert>
#include <cmath>
#include <limits>
#include <utility>  // pair

#include "Modbus/DataStores/DataStore.h"

namespace Modbus {
class RegisterDataStore : public DataStore {
  std::pair<uint16_t*, size_t> data_store_;

 protected:
  static std::size_t GetIndex(const uint16_t address) {
    return address - GetAddressStart();
  }

 public:
  RegisterDataStore(uint16_t* data, const size_t length)
      : data_store_{data, length} {}
  bool ReadLocationValid(std::size_t address, std::size_t count) const {
    return IsAddressValid(address) && IsAddressValid(address + count - 1);
  }
  bool WriteLocationValid(std::size_t address, std::size_t count) const {
    return ReadLocationValid(address, count);
  }
  bool IsAddressValid(std::size_t address) const {
    return address + 1 <= GetAddressStart() + GetSize();
  }
  std::size_t size(void) { return GetSize(); }
  std::size_t GetSize(void) const { return data_store_.second; }
  static constexpr std::size_t GetRegisterByteSize(void) {
    return sizeof(uint16_t);
  }
  uint16_t GetRegister(const std::size_t address) const {
    const size_t index = GetIndex(address);
    assert(index < data_store_.second);
    return data_store_.first[index];
  }

  void GetRegisters(const std::size_t address, const std::size_t register_count,
                    ArrayView<uint8_t>* data_view) {
    const std::size_t num_data_bytes = register_count * sizeof(uint16_t);
    std::size_t register_index = address;
    for (std::size_t byte_number = 0; byte_number < num_data_bytes;) {
      const uint16_t state = GetRegister(register_index++);
      data_view->operator[](byte_number++) = Utilities::GetByte(state, 1);
      data_view->operator[](byte_number++) = Utilities::GetByte(state, 0);
    }
  }

  //  void set_register_callback(std::size_t, uint16_t) {}
  //  void set_registers_callback(std::size_t, std::size_t, const
  //  ArrayView<const uint8_t>&) {}

  void SetRegister(std::size_t address, uint16_t value) {
    data_store_.first[GetIndex(address)] = value;
  }

  void SetRegisters(std::size_t address, std::size_t register_count,
                    const ArrayView<const uint8_t>& data_view) {
    for (std::size_t index = 0; index < register_count; index++) {
      const std::size_t byte_index = index * sizeof(uint16_t);
      const ArrayView<const uint8_t> uint16_data_view{sizeof(uint16_t),
                                                      &data_view[byte_index]};
      const uint16_t value =
          Utilities::Make_MSB_uint16_tFromU8Array(uint16_data_view);
      SetRegister(address + index, value);
    }
  }
};

inline bool is_true(const bool value) { return value; }

class UpdatingRegisterDataStore : public RegisterDataStore {
  std::pair<bool*, size_t> flags_;

 public:
  UpdatingRegisterDataStore(uint16_t* data, const size_t length, bool* flags)
      : RegisterDataStore{data, length}, flags_{flags, length} {}

  bool flag_set(const std::size_t address) const {
    return flags_.first[std::min(address, flags_.second - 1)];
  }

  void set_register_callback(std::size_t address, uint16_t) {
    flags_.first[GetIndex(address)] = true;
  }

  void set_registers_callback(std::size_t address, std::size_t register_count,
                              const ArrayView<const uint8_t>&) {
    for (size_t i = 0; i < register_count; i++) {
      flags_.first[GetIndex(address + i)] = true;
    }
  }

  bool new_data_available() const {
    return std::any_of(flags_.first, flags_.first + flags_.second - 1, is_true);
  }

  void clear_new_data_flags() {
    std::fill(flags_.first, flags_.first + flags_.second - 1, false);
  }
};
}  //  namespace Modbus
