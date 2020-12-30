/*
 * Copyright 2020 ElectroOptical Innovations, LLC
 *
 */

#pragma once
#ifndef UTILITIES_TYPECONVERSION_H_
#define UTILITIES_TYPECONVERSION_H_

#include <ArrayView/ArrayView.h>
#include <algorithm>
#include <array>
#include <limits>
#include <cassert>
#include <cstdint>

namespace Utilities {
template<typename T = int>
constexpr T SignExtend(const T data, const std::size_t bits) {
  const T sign_mask = ~((1 << bits)-1);
  const T converted = (data & (1 << (bits - 1)))
          ? data | sign_mask : data;
  return converted;
}

inline constexpr std::size_t CalcNumberOfBytesToFitBits(
    const std::size_t coil_count) {
  const std::size_t kByteSize = 8;
  const std::size_t num_bytes_truncated = coil_count / kByteSize;
  const std::size_t num_bytes_partial =
      (coil_count == num_bytes_truncated * kByteSize) ? 0 : 1;
  return num_bytes_truncated + num_bytes_partial;
}

template<std::size_t byte_swap_count, typename T>
void SwitchByteOrder(T& data) {
  assert(data.size() % byte_swap_count == 0);
  for (std::size_t i = 0; i < data.size(); i += byte_swap_count) {
    std::array<uint8_t, byte_swap_count> buffer{};
    for (std::size_t byte = 0; byte < byte_swap_count; byte++) {
      buffer[byte] = data[i + byte];
    }
    for (std::size_t byte = 0; byte < byte_swap_count; byte++) {
      data[i + byte] = buffer[byte_swap_count - 1 - byte];
    }
  }
}

template <typename T, typename U = std::size_t>
constexpr uint8_t GetByte(const T data, std::size_t byte) {
  assert(sizeof(T) <= sizeof(U));
  assert(sizeof(T) > byte);
  return 0xff & (static_cast<U>(data) >> (byte * 8));
}

[[deprecated]] inline constexpr uint16_t MakeUint16(uint8_t msb, uint8_t lsb) {
  return static_cast<uint16_t>(lsb + (static_cast<uint16_t>(msb) << 8));
}

template<typename T>
inline uint16_t
Make_MSB_uint16_tFromU8Array(const T &data) {
	assert(data.size() >= sizeof(uint16_t));
  return static_cast<uint16_t>(data[1] + (static_cast<uint16_t>(data[0]) << 8));
}
template<typename T>
[[deprecated]]
inline uint16_t
Make_MSB_uint16tFromu8Array(const T &data) {
  return Make_MSB_uint16_tFromU8Array(data);
}

template<typename T>
inline uint16_t
Make_LSB_uint16_tFromU8Array(const T &data) {
	assert(data.size() >= sizeof(uint16_t));
  return static_cast<uint16_t>(data[0] + (static_cast<uint16_t>(data[1]) << 8));
}
template<typename T>
[[deprecated]]
inline uint16_t
Make_LSB_uint16tFromu8Array(const T &data) {
  return Make_LSB_uint16_tFromU8Array(data);
}
template<typename T, typename U>
inline constexpr T
Make_LSB_IntegerTypeFromArray(const U *const data,
                             const std::size_t data_length) {
  static_assert(std::is_integral<T>());
  static_assert(std::is_integral<U>());
  assert(sizeof(T) >= data_length*sizeof(U));
  if (data_length > sizeof(T)*sizeof(U)) {
    return 0;
  }

  T out = 0;
  const constexpr std::size_t kDataSizeInBits = 8 * sizeof(U);
  for (std::size_t i = 0; i < data_length; i++) {
    std::size_t shift = (i) * kDataSizeInBits;
    out += static_cast<T>((data[i] << shift));
  }
  return out;
}

template<typename T>
inline constexpr T
Make_LSB_IntegerTypeFromU8Array(const uint8_t *const data,
                             const std::size_t data_length) {
  return Make_LSB_IntegerTypeFromArray<T, uint8_t>(data, data_length);
}

template<typename T, typename U>
inline constexpr T
Make_MSB_IntegerTypeFromArray(const U *const data,
                             const std::size_t data_length) {
  static_assert(std::is_integral<T>());
  static_assert(std::is_integral<U>());
  assert(sizeof(T) >= data_length*sizeof(U));
  if (data_length > sizeof(T)*sizeof(U)) {
    return 0;
  }

  T out = 0;
  const constexpr std::size_t kDataSizeInBits = 8 * sizeof(U);
  for (std::size_t i = 0; i < data_length; i++) {
    std::size_t shift = ((data_length - 1) - i) * kDataSizeInBits;
    out += static_cast<T>((data[i] << shift));
  }
  return out;
}

template<typename T>
inline constexpr T
Make_MSB_IntegerTypeFromU8Array(const uint8_t *const data,
                             const std::size_t data_length) {
  return Make_MSB_IntegerTypeFromArray<T, uint8_t>(data, data_length);
}


inline constexpr uint32_t
Make_MSB_uint32_tFromU8Array(const uint8_t *const data) {
  return data[3] | data[2] << 8 | data[1] << 16 | data[0] << 24;
}

inline constexpr uint32_t
Make_MSB_uint32_tFromU8Array(const uint8_t *const data,
                             const std::size_t data_length) {
  //  first byte is shifted by 24 etc
  assert(sizeof(uint32_t) >= data_length);
  if (data_length > sizeof(uint32_t))
    return 0;
  uint32_t out = 0;
  const constexpr std::size_t kByteSizeInBits = 8;
  for (std::size_t i = 0; i < data_length; i++) {
    std::size_t shift = ((data_length - 1) - i) * kByteSizeInBits;
    out += (data[i] << shift);
  }
  return out;
}

inline constexpr uint32_t
Make_LSB_uint32_tFromU8Array(const uint8_t *const data) {
  return data[0] | data[1] << 8 | data[2] << 16 | data[3] << 24;
}

inline constexpr uint32_t
Make_LSB_uint32_tFromU8Array(const uint8_t *const data,
                             const std::size_t data_length) {
  //  first byte is shifted by 24 etc
  assert(sizeof(uint32_t) >= data_length);
  if (data_length > sizeof(uint32_t))
    return 0;
  uint32_t out = 0;
  const constexpr std::size_t kByteSizeInBits = 8;
  for (std::size_t i = 0; i < data_length; i++) {
    out += data[i] << (i * kByteSizeInBits);
  }
  return out;
}

inline constexpr uint32_t Make_uint32_tFromU8Array(const uint8_t *const data,
                                                const std::size_t data_length) {
  return Make_LSB_uint32_tFromU8Array(data, data_length);
}
inline constexpr int32_t
Make_LSB_int32_tFromU8Array(const uint8_t *const data,
                            const std::size_t data_length) {
  return Make_LSB_uint32_tFromU8Array(data, data_length);
}
inline constexpr int32_t
Make_MSB_int32_tFromU8Array(const uint8_t *const data,
                            const std::size_t data_length) {
  return Make_MSB_uint32_tFromU8Array(data, data_length);
}
inline constexpr int32_t Make_int32_tFromU8Array(const uint8_t *const data,
                                                const std::size_t data_length) {
  return Make_LSB_uint32_tFromU8Array(data, data_length);
}

template <typename T, std::size_t kBytes>
inline constexpr std::array<uint8_t, kBytes> MakeLSBU8Array(T t) {
  static_assert(kBytes >= sizeof(T), "Type size issue");
  std::array<uint8_t, kBytes> return_array{};
  for (std::size_t byte = 0; byte < kBytes; byte++) {
    return_array[byte] = Utilities::GetByte(t, byte);
  }
  return return_array;
}

template <typename T>
inline constexpr std::array<uint8_t, sizeof(T)> MakeLSBU8Array(T t) {
  return MakeLSBU8Array<T, sizeof(T)>(t);
}

template <typename T, std::size_t kBytes>
inline constexpr std::array<uint8_t, kBytes> MakeMSBU8Array(T t) {
  static_assert(kBytes >= sizeof(T), "Type size issue");
  std::array<uint8_t, kBytes> return_array{};
  for (std::size_t byte = 0; byte < kBytes; byte++) {
    return_array[kBytes - byte - 1] = Utilities::GetByte(t, byte);
  }
  return return_array;
}

template <typename T>
inline constexpr std::array<uint8_t, sizeof(T)> MakeMSBU8Array(T t) {
  return MakeMSBU8Array<T, sizeof(T)>(t);
}

#if 0
template <typename T>
inline constexpr const std::array<uint8_t, sizeof(uint32_t)>
MakeMSBU8Array(T t) {
  static_assert(sizeof(uint32_t) >= sizeof(T), "Type size issue");
  const uint32_t cast = static_cast<uint32_t>(t);
  return std::array<uint8_t, sizeof(uint32_t)>{
      static_cast<uint8_t>(0xff & (cast >> 24)),
      static_cast<uint8_t>(0xff & (cast >> 16)),
      static_cast<uint8_t>(0xff & (cast >> 8)),
      static_cast<uint8_t>(0xff & cast),
  };
}
#endif
template <typename T>
[[deprecated]]
inline constexpr std::array<uint8_t, sizeof(uint32_t)> MakeU8Array(T t) {
  return MakeLSBU8Array<T>(t);
}

inline constexpr void
num2chars(uint32_t num,
          std::array<char, std::numeric_limits<uint32_t>::digits10> &buff) {
  const int base = 10;
  for (auto entry = buff.rbegin(); entry < buff.rend(); ++entry) {
    const auto rem = static_cast<char>((num - (num / base) * base) & (0xff));
    num = num / base;
    *entry = static_cast<char>(('0' + rem) & 0xff);
  }
}

template <typename T, typename U>
inline constexpr T StaticCastQuickFail(U x) {
  if (std::is_same<T, U>()) {
    return x;
  }
  assert(x >= std::numeric_limits<T>::lowest());
  assert(x <= std::numeric_limits<T>::max());
  return static_cast<T>(x);
}

template<typename T>
void ArrayToBytes(const T * const data_in, uint8_t * const data_out, const std::size_t size_in, const std::size_t size_out) {
  std::size_t data_out_count = 0;
  for (std::size_t i = 0; i < std::min(size_in, size_out/sizeof(T)); i++) {
    const auto arr = MakeMSBU8Array<T>(data_in[i]);
    for (std::size_t j = 0; j < arr.size(); j++) {
      data_out[i*sizeof(T) + j] = arr[j];
    }
  }
}

template<typename T>
void ArrayFromBytes(const uint8_t * const data_in, T * const data_out, const std::size_t size_in, const std::size_t size_out) {
  //#ERROR
  assert(0);
  //static_assert(0, "Undefined");
}

template<>
inline void ArrayFromBytes<uint16_t>(const uint8_t * const data_in, uint16_t * const data_out, const std::size_t size_in, const std::size_t size_out) {
  std::size_t data_out_count = 0;
  for (std::size_t i = 0; i < std::min(size_in/sizeof(uint16_t), size_out); i++) {
    data_out[i] = Make_MSB_uint32_tFromU8Array(&data_in[i*sizeof(uint16_t)], sizeof(uint16_t));
  }
}
}  //  namespace Utilities
#endif  //  UTILITIES_TYPECONVERSION_H_
