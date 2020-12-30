#pragma once
#include <ArrayView/ArrayView.h>
#include <cstdint>
namespace Modbus {
template<typename T>
void MakeRegistersToBytes(const T& data_view, ArrayView<uint8_t> data_out) {
  for (std::size_t i = 0; i < data_view.size(); i++) {
    data_out[i*sizeof(uint16_t)] = Utilities::GetByte(data_view.at(i), 1);
    data_out[i*sizeof(uint16_t) + 1] = Utilities::GetByte(data_view.at(i), 0);
  }
}
}

