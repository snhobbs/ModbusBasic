#pragma once
#include <utility>  //  for std::pair

#include "ByteField.h"

inline bool fields_are_sorted(const U8Field* const* const fields,
                              const size_t length) {
  bool sorted = true;
  size_t next_start = 0;
  for (size_t i = 0; i < length; i++) {
    if (fields[i]->address < next_start) {
      sorted = false;
      break;
    }
    next_start = fields[i]->address + fields[i]->length;
  }
  return sorted;
}

/*
 * Accessor for a field of values with addresses, data blocks, and sizes
 * Blocks can be scattered and do not have to be continuous
 * */
class ByteFieldController {
  std::pair<U8Field**, size_t>
      entries_;  //  List of blocks with address, size, and data buffer

 public:
  /*
   * find_block
   * Crawls through the list of block enries, returns the index of the block
   * if the starting address is within the range of that block
   * */
  size_t find_block(const size_t address, const size_t) const {
    size_t block = entries_.second;  //  impossible value
    for (size_t i = 0; i < entries_.second; i++) {
      const size_t block_end =
          entries_.first[i]->address + entries_.first[i]->length - 1;
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
    const auto p_block = entries_.first[block];
    const size_t block_end = p_block->address + p_block->length - 1;
    const size_t read_end = address + length - 1;
    if (address <= block_end) {  //  start is less than the end of the block
      if ((address >= p_block->address) && (read_end <= block_end)) {
        valid = true;
      }
    }
    return valid;
  }

  ByteFieldController(U8Field** entries, const size_t entry_length)
      : entries_{entries, entry_length} {
    assert(fields_are_sorted(entries, entry_length));
  }
};
