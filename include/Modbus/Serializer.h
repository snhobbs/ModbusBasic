#pragma once
#include <cstddef>
#include <cstdint>
#include <cassert>
#include <climits>
#include <type_traits>

template<typename T, bool kIsFundemental = std::is_trivially_copyable<T>::value>
class Serializer {
 public:
  static const constexpr size_t kStep = 0;
  static constexpr size_t serialize(uint8_t* storage, const size_t storage_length, const T& data) {
    assert(0);
    return 0;
  }
  static constexpr size_t deserialize(const uint8_t* storage, const size_t storage_length, T* data) {
    assert(0);
    return 0;
  }
};

template<typename T>
class Serializer<T, true> {
 public:
  static const constexpr size_t kStep = sizeof(T);
  static constexpr size_t serialize(uint8_t* storage, const size_t storage_length, const T& data) {
    const uint8_t* t_cast = reinterpret_cast<const uint8_t*>(&data);
    size_t bytes_written = 0;
    if (storage_length >= kStep) {
      for (bytes_written=0; bytes_written<kStep; bytes_written++) {
        storage[bytes_written] = t_cast[bytes_written];
      }
      bytes_written++;
    }
    return bytes_written;
  }
  static constexpr size_t deserialize(const uint8_t* storage, const size_t storage_length, T* data) {
    size_t bytes_written = 0;
    if (storage_length >= kStep) {
      uint8_t* p_cast_t = reinterpret_cast<uint8_t*>(data);
      for (bytes_written=0; bytes_written<kStep; bytes_written++) {
        p_cast_t[bytes_written] = storage[bytes_written];
      }
      bytes_written++;
    }
    return bytes_written;
  }
};

template<typename T>
void write_as_type(uint8_t* storage, const size_t storage_length, const T* data, const size_t data_length) {
  if (data_length * Serializer<T>::kStep < storage_length) {
    assert(0);
  } else {
    for (size_t i=0; i<data_length; i++) {
      const size_t offset = Serializer<T>::kStep * i;
      Serializer<T>::serialize(&(storage[offset]), storage_length, data[i]);
    }  
  }
}

template<typename T>
void read_as_type(const uint8_t* storage, const size_t storage_length, T* data, const size_t data_length) {
  if (data_length * Serializer<T>::kStep < storage_length) {
    assert(0);
  } else {
    for (size_t i=0; i<data_length; i++) {
      const size_t offset = Serializer<T>::kStep * i;
      Serializer<T>::deserialize(&(storage[offset]), storage_length, &(data[i]));
    }
  }
}
