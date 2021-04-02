/*
 * Copyright 2020 Electrooptical Innovations, LLC
 * Holding register controller
 *
 * -> Be able to read out registers normally
 * -> Validate on aligned writes
 * -> Struct mapped onto registers
 *  each entry must be aligned to 16 bits
 *
 *  This wrapper takes an object that has:
 *    - Number of fields (number_of_fields)
 *    - Offset of entries (get_offset)
 *      - Request the starting offset of entry with an index 
 *    - Length of entries (get_field_size)
 *      - Request the full size of entry with an index 
 *    - Read bytes (read)
 *      - Takes starting address, length, and a pointer to a buffer 
 *    - Write bytes (write)
 *      - Same as read
 * */
#pragma once
//  #include <Modbus/DataCommand.h>
#include <Modbus/DataStore.h>
#include <Modbus/Modbus.h>
#include <algorithm>
#include <cstddef>
#include <cstdint>
namespace Modbus {
template <typename T> class MappedRegisterDataStore : public DataStore {
  bool new_data_ = false;
  T *memory_controller_;

public:
  explicit MappedRegisterDataStore(T *memory_controller)
      : memory_controller_{memory_controller} {}

  bool IsNewData(void) const { return new_data_; }
  void SetNewData(bool value) { new_data_ = value; }

  size_t GetSize(void) const {
    return memory_controller_->size();
  }
  size_t size(void) const {
    return memory_controller_->size();
  }
  static constexpr std::size_t GetRegisterByteSize(void) {
    return sizeof(uint16_t);
  }

  uint16_t GetRegister(std::size_t address) const {
    std::array<uint8_t, sizeof(uint16_t)> data{};
    if (RegisterSpanValid(address, 1)) {
      read(address*sizeof(uint16_t), sizeof(uint16_t), data.data());
    }
    const uint16_t value = static_cast<uint16_t>(data[0]) << 8 | data[1];
    return value;
  }

  //  [[deprecated]]
  template<typename F>
  void GetRegisters(const size_t address, const size_t register_count, F* data) const {
    GetRegisters(address, register_count, data->data(), data->size());
  }

  void GetRegisters(const size_t address, const size_t register_count, uint8_t* data, const size_t length) const {
    const auto byte_address = sizeof(uint16_t)*address;
    const auto byte_count = sizeof(uint16_t)*register_count;
    if (RegisterSpanValid(address, register_count) && byte_count <= length) {
      read(byte_address, byte_count, data);
    } else {
      assert(0);
    }
  }

  void SetRegister(std::size_t address, const uint16_t value) {
    std::array<uint8_t, sizeof(uint16_t)> data{0xFF&(value >> 8), 0xFF&value};
    SetRegisters(address, 1, data.data(), data.size());
  }

  //  Validation has made each one of the a complete entry to one of our values
  //[[deprecated]]
  template<typename F>
  void SetRegisters(const size_t address, const size_t register_count, F &data) {
    SetRegisters(address, register_count, data.data(), data.size());
  }

  void SetRegisters(const size_t address, const size_t register_count, const uint8_t* data, const size_t length) {
    const auto byte_address = sizeof(uint16_t)*address;
    const auto byte_count = sizeof(uint16_t)*register_count;
    if (RegisterSpanValid(address, register_count) && byte_count <= length) {
      write(byte_address, byte_count, data);
    } else {
      assert(0);
    }
  }

  bool RegisterSpanValid(const size_t address, const size_t count) const {
    return ReadLocationValid(sizeof(uint16_t)*address, sizeof(uint16_t)*count);
  }

 //private:
  bool ReadLocationValid(std::size_t byte_address, std::size_t byte_count) const {
	return memory_controller_->ReadLocationValid(byte_address, byte_count);
  }

  bool WriteLocationValid(std::size_t byte_address, std::size_t byte_count) const {
	const bool valid = memory_controller_->WriteLocationValid(byte_address, byte_count);
    return valid;
  }

  void write(const size_t byte_address, const size_t count, const uint8_t* data) {
    SetNewData(true);
    memory_controller_->write(byte_address, count, data);
  }

  void read(const size_t byte_address, const size_t count, uint8_t* data) const {
    return memory_controller_->read(byte_address, count, data);
  }
};  //  class MappedRegisterDataStore

} //  namespace Modbus
