#pragma once
#include <cstddef>
#include <cstdint>
#include <cassert>
#include <cstring>
#include <climits>
#include <algorithm>
#include "Modbus/Serializer.h"

#define BYTE_SIZE 8
#define REGISTER_SIZE 2

struct u8_Buffer {
  uint8_t* buffer;
  size_t size;
};

struct BitField {
  size_t address;
  size_t length;
  u8_Buffer* buffer;
};

typedef BitField RegisterField;

inline bool fields_are_sorted(const BitField * const * const fields, const size_t length) {
  bool sorted = true;
  size_t next_start = 0;
  for (size_t i=0; i<length; i++) {
    if (fields[i]->address < next_start) {
      sorted = false;
      break;
    }
    next_start = fields[i]->address + fields[i]->length;
  }
  return sorted;
}

class Controller {
  BitField** entries_;
  size_t entry_length_;

 public:
  size_t find_block(const size_t address, const size_t) const {
    size_t block = entry_length_;  //  impossible value
    for (size_t i=0; i<entry_length_; i++) {
      const size_t block_end = entries_[i]->address + entries_[i]->length - 1;
      if (address <= block_end) {
        block = i;
        break;
      }
    }
    return block;
  }

  bool access_valid(const size_t address, const size_t length) const {
    bool valid = false;
    const size_t block = find_block(address, length);
    const size_t block_end = entries_[block]->address + entries_[block]->length - 1;
    const size_t read_end = address + length - 1;
    if (address <= block_end) {  //  start is less than the end of the block 
      if ( ( address >= entries_[block]->address ) && ( read_end <= block_end ) ) {
        valid = true;
      }
    }
    return valid;
  }

  Controller(BitField** entries, const size_t entry_length) : entries_{entries}, entry_length_{entry_length} {
    assert(fields_are_sorted(entries_, entry_length_));
  }
};
