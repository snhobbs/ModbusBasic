#pragma once
#include <cstddef>
#include <cstdint>
#include <cassert>
#include <cstring>
#include <climits>
#include <algorithm>
#include "Modbus/ModbusNew.h"

namespace Modbus {
class RegisterController {
  RegisterField** entries_;
  size_t entry_length_;
  Controller controller_;

 public:
  bool access_valid(const size_t address, const size_t length) const {
    return controller_.access_valid(address, length);
  }

  size_t read(const size_t address, const size_t length, u8_Buffer* buffer) const {
    size_t i = 0;  //  number of fields read will be returned
    if (access_valid(address, length)) {
      const size_t block = controller_.find_block(address, length);
      const size_t max_byte_index = std::min(buffer->size, entries_[block]->buffer->size);
      const size_t transfer_length = std::min(max_byte_index/REGISTER_SIZE, length);
      assert(transfer_length > 0);
      for (i=0; i<transfer_length; i++) {  //  length is number of registers which are 16 bits
        const size_t byte_number = REGISTER_SIZE * (i);
        const size_t entry_address = address + byte_number;
        buffer->buffer[byte_number] = entries_[block]->buffer->buffer[entry_address];
        buffer->buffer[byte_number + 1] = entries_[block]->buffer->buffer[entry_address + 1];
      }
    }
    return i;
  }

  size_t write(const size_t address, const size_t length, const u8_Buffer* buffer) {
    size_t i = 0;  //  number of fields written will be returned
    if (access_valid(address, length)) {
      const size_t block = controller_.find_block(address, length);
      const size_t max_byte_index = std::min(buffer->size, entries_[block]->buffer->size);
      const size_t transfer_length = std::min(max_byte_index/REGISTER_SIZE, length);
      assert(transfer_length > 0);
      for (i=0; i<transfer_length; i++) {  //  length is number of registers which are 16 bits
        const size_t byte_number = REGISTER_SIZE * (i);
        const size_t entry_address = address + byte_number;
        entries_[block]->buffer->buffer[entry_address] = buffer->buffer[byte_number];
        entries_[block]->buffer->buffer[entry_address + 1] = buffer->buffer[byte_number + 1];
      }
    }
    return i;
  }

  RegisterController(RegisterField** entries, const size_t entry_length) :
    entries_{entries}, entry_length_{entry_length}, controller_{entries, entry_length} {}
};
}
