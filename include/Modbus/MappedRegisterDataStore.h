/*
 * Copyright 2020 Electrooptical Innovations, LLC
 * Holding register controller
 *
 * -> Be able to read out registers normally
 * -> Validate on aligned writes
 * -> Struct mapped onto registers
 *  each entry must be aligned to 16 bits
 * */
#pragma once
#include <Modbus/DataCommand.h>
#include <Modbus/DataStores/DataStore.h>
#include <Modbus/Modbus.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <utility>  //  std::pair

template <typename T>
struct Equal_UnaryPredicate {
  const T d_;
  Equal_UnaryPredicate(const T &n) : d_{n} {}
  bool operator()(const T &n) const { return n == d_; }
};

namespace Modbus {
struct BasicMemoryMapEntry {
  std::size_t offset;
  std::size_t size;
};

template <typename T>
struct MemoryMapEntry {
  T identifier;
  std::size_t offset;
  std::size_t size;
};

template <typename T>
struct IdentifierMemoryMapEntry {
  T identifier;
  std::size_t offset;
  std::size_t size;
};

template <typename T>
inline size_t get_matching_index(
    const T &value, const std::pair<const T *, size_t> &collection) {
  /*
   * Cycle through the buffer, check if the value matches, if it does, return
   * the index. Return length of buffer if unfound.
   * */
  size_t i = 0;
  for (i = 0; i < collection.second; i++) {
    if (collection.first[i] == value) {
      break;
    }
  }
  return i;
}

inline bool check_location_valid(
    std::size_t address, std::size_t count,
    const std::pair<const size_t *, size_t> &offsets,
    const std::pair<const size_t *, size_t> &end_points) {
  /*
   * Arguments:
   *  + address : start location
   *  + count : number of registers
   *
   *  Only for complete writes of registers. Allow a write if the address is
   * in start positions and the end point of address + count is in end points
   * */
  const bool valid =
      std::any_of(offsets.first, offsets.first + offsets.second,
                  Equal_UnaryPredicate(address)) &
      std::any_of(end_points.first, end_points.first + end_points.second,
                  Equal_UnaryPredicate(address + count - 1));
  return valid;
}

template <typename T>
class MappedRegisterDataStore : public DataStore {
  bool new_data_ = false;
  T *memory_controller_;

 public:
  explicit MappedRegisterDataStore(T *memory_controller)
      : memory_controller_{memory_controller} {}

  std::size_t GetMemoryMapEntryIndex(std::size_t address) const {
    /*
     * Returns the index of the matching address. Returns 0 otherwise.
     * */
    const auto index =
        get_matching_index(address, {memory_controller_->offsets_.cbegin(),
                                     memory_controller_->offsets_.size()});
    return (index == memory_controller_->offsets_.size()) ? 0 : index;
  }

  bool IsNewData(void) const { return new_data_; }
  void SetNewData(bool value) { new_data_ = value; }
  void SetField(const std::size_t identifier,
                const std::pair<const uint8_t *, size_t> &data_view) {
    SetNewData(true);
    memory_controller_->SetField(identifier, data_view.first, data_view.second);
  }
  void SetField(const std::size_t identifier,
                const ArrayView<const uint8_t> &data_view) {
    SetNewData(true);
    memory_controller_->SetField(identifier, data_view.data(),
                                 data_view.size());
  }

  void GetField(const std::size_t identifier,
                ArrayView<uint8_t> *data_view) const {
    return memory_controller_->GetField(identifier, data_view->data(),
                                        data_view->size());
  }

 public:
  static constexpr std::size_t size(void) { return T::size(); }
  static constexpr std::size_t GetSize(void) { return size(); }
  static constexpr std::size_t GetRegisterByteSize(void) {
    return sizeof(uint16_t);
  }
  std::size_t GetIdentifierFromAddress(std::size_t address) const {
    return GetMemoryMapEntryIndex(address);
  }

  bool ReadLocationValid(std::size_t address, std::size_t count) const {
    return WriteLocationValid(address, count);
  }

  bool WriteLocationValid(std::size_t address, std::size_t count) const {
    /*
     * Arguments:
     *  + address : start location
     *  + count : number of registers
     *
     *  Only for complete writes of registers. Allow a write if the address is
     * in start positions and the end point of address + count is in end points
     * */
    const bool valid =
        check_location_valid(address, count,
                             {memory_controller_->offsets_.cbegin(),
                              memory_controller_->offsets_.size()},
                             {memory_controller_->end_points_.cbegin(),
                              memory_controller_->end_points_.size()});
    return valid;
  }

  template <typename F>
  void SetFieldFromAddress(std::size_t address, const F &data_view) {
    const auto identifier = GetIdentifierFromAddress(address);
    SetField(identifier, data_view);
  }
  void GetFieldFromAddress(std::size_t address,
                           ArrayView<uint8_t> *data_view) const {
    const auto identifier = GetIdentifierFromAddress(address);
    GetField(identifier, data_view);
  }

  uint16_t GetRegister(std::size_t address) const {
    std::array<uint8_t, sizeof(uint16_t)> data{};
    ArrayView data_view{data.size(), data.data()};
    GetRegisters(address, 1, &data_view);
    const uint16_t value = Utilities::Make_MSB_uint16_tFromU8Array(data_view);
    return value;
  }

  void GetRegisters(std::size_t address, std::size_t register_count,
                    ArrayView<uint8_t> *data_view) const {
    assert(ReadLocationValid(address, register_count));
    GetFieldFromAddress(address, data_view);
  }

  void SetRegister(std::size_t address, uint16_t value) {
    std::array<uint8_t, sizeof(uint16_t)> data{Utilities::GetByte(value, 1),
                                               Utilities::GetByte(value, 0)};
    const ArrayView<const uint8_t> uint16_data_view{data.size(), data.data()};
    SetRegisters(address, 1, uint16_data_view);
  }

  //  Validation has made each one of the a complete entry to one of our values
  void SetRegisters(std::size_t address, std::size_t register_count,
                    const ArrayView<const uint8_t> &data_view) {
    assert(WriteLocationValid(address, register_count));
    SetFieldFromAddress(address, data_view);
  }
};  //  class MappedRegisterDataStore

}  //  namespace Modbus
