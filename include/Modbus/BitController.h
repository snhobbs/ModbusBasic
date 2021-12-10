#pragma once
#include <cstddef>
#include <cstdint>
#include <cassert>
#include <cstring>
#include <climits>
#include <algorithm>
#include <Modbus/ModbusNew.h>

class BitController {
  BitField** entries_;
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
      for (i=0; i<length; i++) {
        const size_t byte_index = i%BYTE_SIZE;
        const size_t byte_number = (i/BYTE_SIZE);
        const uint8_t masked_byte_value = entries_[block]->buffer->buffer[byte_number] & (1<<(byte_index));   //  Clear all but the bit we're setting
        if (byte_number > std::min(buffer->size, entries_[block]->buffer->size)) {
          break;  //  Do not overrun either buffer;
        } else {
          const uint8_t byte = buffer->buffer[byte_number] & ~(1<<(byte_index)) | masked_byte_value;
          buffer->buffer[byte_number] = byte;
        }
      }
    }
    return i;
  }

  size_t write(const size_t address, const size_t length, const u8_Buffer* buffer) {
    size_t i = 0;  //  number of fields read will be returned
    if (access_valid(address, length)) {
      const size_t block = controller_.find_block(address, length);
      for (i=0; i<length; i++) {
        const size_t byte_index = i%BYTE_SIZE;
        const size_t byte_number = (i/BYTE_SIZE);
        const uint8_t masked_byte_value = buffer->buffer[byte_number] & (1<<(byte_index));   //  Clear all but the bit we're setting
        if (byte_number > std::min(buffer->size, entries_[block]->buffer->size)) {
          break;  //  Do not overrun either buffer;
        } else {
          const uint8_t byte = entries_[block]->buffer->buffer[byte_number] & ~(1<<(byte_index)) | masked_byte_value;
          entries_[block]->buffer->buffer[byte_number] = byte;
          assert(entries_[block]->buffer->buffer[byte_number] == byte);
        }
      }
    }
    return i;
  }

  BitController(BitField** entries, const size_t entry_length) :
    entries_{entries}, entry_length_{entry_length}, controller_{entries, entry_length} {}
};


