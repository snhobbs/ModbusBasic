#pragma once
#include <algorithm>
#include <cassert>
#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <utility>  //  for std::pair

#include "Modbus/ByteField.h"
#include "Modbus/ByteFieldController.h"

namespace Modbus {
class RegisterController {
  std::pair<U8Field**, size_t> entries_;
  ByteFieldController controller_;

 public:
  bool access_valid(const size_t address, const size_t length) const {
    return controller_.access_valid(address, length);
  }

  size_t read(const size_t address, const size_t length,
              u8_Buffer* buffer) const {
    size_t i = 0;  //  number of fields read will be returned
    if (access_valid(address, length)) {
      const size_t block = controller_.find_block(address, length);
      auto p_block = entries_.first[block];
      const size_t max_byte_index =
          std::min(buffer->size, p_block->buffer->size);
      const size_t transfer_length =
          std::min(max_byte_index / REGISTER_SIZE, length);
      assert(transfer_length > 0);
      for (i = 0; i < transfer_length;
           i++) {  //  length is number of registers which are 16 bits
        const size_t byte_number = REGISTER_SIZE * (i);
        const size_t entry_address = address + byte_number;
        buffer->buffer[byte_number] = p_block->buffer->buffer[entry_address];
        buffer->buffer[byte_number + 1] =
            p_block->buffer->buffer[entry_address + 1];
      }
    }
    return i;
  }

  size_t write(const size_t address, const size_t length,
               const u8_Buffer* buffer) {
    size_t i = 0;  //  number of fields written will be returned
    if (access_valid(address, length)) {
      const size_t block = controller_.find_block(address, length);
      auto p_block = entries_.first[block];
      const size_t max_byte_index =
          std::min(buffer->size, p_block->buffer->size);
      const size_t transfer_length =
          std::min(max_byte_index / REGISTER_SIZE, length);
      assert(transfer_length > 0);
      for (i = 0; i < transfer_length;
           i++) {  //  length is number of registers which are 16 bits
        const size_t byte_number = REGISTER_SIZE * (i);
        const size_t entry_address = address + byte_number;
        p_block->buffer->buffer[entry_address] = buffer->buffer[byte_number];
        p_block->buffer->buffer[entry_address + 1] =
            buffer->buffer[byte_number + 1];
      }
    }
    return i;
  }

  RegisterController(U8Field** entries, const size_t entry_length)
      : entries_{entries, entry_length}, controller_{entries, entry_length} {}
};
}  // namespace Modbus
