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
#include <cstdint>
#include <cstddef>
#include <Modbus/Modbus.h>
#include <Modbus/DataStore.h>
#include <Modbus/DataCommand.h>
#include <algorithm>
namespace Modbus {
template<typename T>
struct MemoryMapEntry {
  T identifier;
  std::size_t offset;
  std::size_t size;
};

template<typename MapObject>
class MappedRegisterDataStore : public DataStore {
  MapObject map_object_;

 public:
  MapObject& GetMapObject(void) {
    return map_object_;
  }

  static constexpr bool WriteLocationValid(std::size_t address, std::size_t count) {
    return MapObject::WriteLocationValid(address - GetAddressStart(), count);
  }

  static constexpr bool ReadLocationValid(std::size_t address, std::size_t count) {
    return MapObject::ReadLocationValid(address - GetAddressStart(), count);
  }

  static std::size_t size(void) {return GetSize();}
  static constexpr std::size_t GetSize(void) { return MapObject::size(); }
  static constexpr std::size_t GetRegisterByteSize(void) { return sizeof(uint16_t); }

  uint16_t GetRegister(std::size_t address) const {
    std::array<uint8_t, sizeof(uint16_t)> data{};
    ArrayView data_view{data.size(), data.data()};
    GetRegisters(address, 1, &data_view);
    const uint16_t value = Utilities::Make_MSB_uint16_tFromU8Array(data_view);
    return value;
  }

  void GetRegisters(std::size_t address, std::size_t register_count, ArrayView<uint8_t>* data_view) const {
    assert(ReadLocationValid(address, register_count));
    map_object_.GetFieldFromAddress(address, data_view);
  }

  void SetRegister(std::size_t address, uint16_t value) {
    std::array<uint8_t, sizeof(uint16_t)> data {
      Utilities::GetByte(value, 1),
      Utilities::GetByte(value, 0)
    };
    const ArrayView<const uint8_t> uint16_data_view{data.size(), data.data()};
    SetRegisters(address, 1, uint16_data_view);
  }

  //  Validation has made each one of the a complete entry to one of our values
  void SetRegisters(std::size_t address, std::size_t register_count, const ArrayView<const uint8_t>& data_view) {
    assert(WriteLocationValid(address, register_count));
    map_object_.SetFieldFromAddress(address, data_view);
  }

  constexpr MappedRegisterDataStore() {}
};

}  //  namespace Modbus
