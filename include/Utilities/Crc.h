/*
 * Copyright 2020 ElectroOptical Innovations, LLC
 * */
#pragma once
#ifndef UTILITIES_CRC_H_
#define UTILITIES_CRC_H_
#include <array>
#include <cstdint>
#include <cassert>

namespace Utilities {

/*
 * lrc := 0
for each byte b in the buffer do
    lrc := (lrc + b) and 0xFF
lrc := (((lrc XOR 0xFF) + 1) and 0xFF)
 *
 * */
inline constexpr uint8_t LinearRedundancyCheck(const uint8_t *const buffer,
                                               const std::size_t data_length) {
  const uint8_t kMask = 0xff;
  uint8_t lrc = 0;
  for (std::size_t current_byte = 0; current_byte < data_length; current_byte++) {
    lrc = static_cast<uint16_t>((lrc + buffer[current_byte]) & kMask);
    lrc = static_cast<uint16_t>(((lrc ^ 0xFF) + 1) & kMask);
  }
  return lrc;
}

inline constexpr uint16_t crc16(const uint8_t *const buffer,
                                const std::size_t data_length) {
  const constexpr std::size_t crc16_polynomial = 0xA001;
  uint16_t crc = 0xffff;
  for (std::size_t current_byte = 0; current_byte < data_length; current_byte++) {
    crc = static_cast<uint16_t>(crc ^ buffer[current_byte]);
    for (uint8_t j = 0; j < 8; j++) {
      if (crc & 0x0001) {
        crc = static_cast<uint16_t>((crc >> 1) ^ crc16_polynomial);
      } else {
        crc = static_cast<uint16_t>(crc >> 1);
      }
    }
  }
  return static_cast<uint16_t>(crc << 8 | crc >> 8);
}

template<typename T>
inline uint16_t crc16(const T &array, std::size_t length) {
  assert(length <= array.size());
  return Utilities::crc16(array.data(), length);
}

struct Crc8Setting {
  const uint8_t polynomial = 0xff;
  const uint8_t initial_value = 0xff;
  const uint8_t final_xor = 0x00;
  const bool reflect_in = false;
  const bool reflect_out = false;
};

inline constexpr uint8_t reflect_byte(const uint8_t data) {
  const std::size_t kByteSize = 8;
  uint8_t result = 0;
  for (std::size_t i = 0; i < kByteSize; i++) {
    result |= (1U & (data >> (kByteSize - i - 1))) << i;
  }
  return result;
}

static_assert(reflect_byte(0b00000001) == 0b10000000);
static_assert(reflect_byte(0b00000011) == 0b11000000);
static_assert(reflect_byte(0b00000111) == 0b11100000);
static_assert(reflect_byte(0b00001111) == 0b11110000);
static_assert(reflect_byte(0b10001111) == 0b11110001);

inline constexpr uint8_t crc_uint8(const uint8_t *const buffer,
      const std::size_t data_length, const Crc8Setting& settings) {
  uint8_t crc = settings.initial_value;
  /* calculates 8-Bit checksum with given polynomial */
  for (std::size_t current_byte = 0; current_byte < data_length; current_byte++) {
    uint8_t byte = buffer[current_byte];
    if (settings.reflect_in) {
      byte = reflect_byte(byte);
    }
    crc ^= (buffer[current_byte]);
    for (std::size_t crc_bit = 8; crc_bit > 0; --crc_bit) {
      const auto crc_shift = static_cast<uint8_t>(crc << 1);
      if (crc & 0x80) {
        crc = (crc_shift) ^ settings.polynomial;
      } else {
        crc = crc_shift;
      }
    }
  }
  if (settings.reflect_out) {
    crc = reflect_byte(crc);
  }
  return crc^settings.final_xor;
}

} //  namespace Utilities

#endif //  UTILITIES_CRC_H_
