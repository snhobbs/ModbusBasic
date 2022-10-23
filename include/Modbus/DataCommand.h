/* Copyright (C) 2020 Electrooptical Innovations
 * ----------------------------------------------------------------------
 * Project:      Modbus
 * Title:        DataCommand.h
 * Description:  Super class for commands requesting data reads
 *
 * $Date:        13. May 2020
 * $Revision:    V.1.0.1
 *
 * Target Processor: Any, Linux system
 * ----------------------------------------------------------------------
 */

#pragma once
#include <ArrayView/ArrayView.h>
#include <Modbus/Modbus.h>
#include <Utilities/CommonTypes.h>
#include <Utilities/TypeConversion.h>

#include <cstdint>
namespace Modbus {
class DataCommand : public Command {
  static const constexpr std::size_t kIntegerSize = sizeof(uint16_t);

 public:
  struct CommandPacket {
    static const constexpr std::size_t kDataAddressStart = 0;
    static const constexpr std::size_t kDataAddressEnd =
        kDataAddressStart + kIntegerSize - 1;
  };

 public:
  static uint16_t ReadAddressStart(const ArrayView<uint8_t> &data_array) {
    return Utilities::Make_MSB_uint16_tFromU8Array(ArrayView<const uint8_t>{
        sizeof(uint16_t), &data_array[CommandPacket::kDataAddressStart]});
  }
};
}  //  namespace Modbus
