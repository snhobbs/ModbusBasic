/*
 *  Wrap a flatbuffers produced class
 *
 * */
#pragma once
#include <cstdint>
#include <cstring>
#include "Modbus/DataStore.h"

namespace Modbus {
template <typename T>
class FlatBufferRegisterStore : public DataStore {
  T* obj_;

 public:
  explicit FlatBufferRegisterStore(T* obj): obj_{obj} {}
  static const size_t number_of_fields = T::Traits::fields_number;
  static std::size_t size(void) {return GetSize();}
  static constexpr std::size_t GetSize(void) { return sizeof(T); }
  static constexpr std::size_t GetRegisterByteSize(void) { return sizeof(uint16_t); }

  //  FIXME This will cause the padding to be readable and writable
  size_t get_field_size(const size_t index) const {
    const size_t offset = obj_->get_offset(index);
    if (index >= number_of_fields-1) {
      return sizeof(T) - offset;
    } else {
      return obj_->get_offset(index+1) - offset;
    }
  }

  size_t get_offset(const size_t index) const {
    return obj_->get_offset(index);
  }

  std::size_t GetMemoryMapEntryIndex(std::size_t byte_address) const {
    for (size_t i=0; i<number_of_fields; i++) {
      if (obj_->get_offset(i) == byte_address) {
        return i;
      }
    }
    return number_of_fields;
  }

  bool ReadLocationValid(std::size_t byte_address, std::size_t byte_count) const {
    return WriteLocationValid(byte_address, byte_count);
  }

  bool WriteLocationValid(std::size_t byte_address, std::size_t byte_count) const {
    const size_t index = GetMemoryMapEntryIndex(byte_address);
    // Since we don't know the size of the object, only where it starts and the next
    // begins, this breaks on the padding. Work around is to allow partial writes
    // and reads.
    const bool valid =  index < number_of_fields && get_field_size(index) >= byte_count;
    if (!valid) {
    	while(1){}
    }
    return valid;
  }

  void read(const size_t byte_address, const size_t count, uint8_t* data) const {
    memcpy(data, reinterpret_cast<const uint8_t*>(obj_)+byte_address, count);
  }

  void write(const size_t byte_address, const size_t count, const uint8_t* data) {
    memcpy(reinterpret_cast<void*>(obj_)+byte_address, data, count);  }
};

}  //  namespace Modbus
