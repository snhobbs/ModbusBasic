#pragma once
namespace Modbus {

template<typename T>  
static bool InList(const T& item, const T * const list, const std::size_t size) {
  for (std::size_t i = 0; i < size; i++) {
    if (item == list[i]) {
      return true;
    }
  }
  return false;
}
}
