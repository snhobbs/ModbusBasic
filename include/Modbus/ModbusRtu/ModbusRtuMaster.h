/* Copyright (C) 2020 Electrooptical Innovations
 * ----------------------------------------------------------------------
 * Project:      Modbus
 * Title:        ModbusRtu/ModbusRtuMaster.h
 * Description:  Template base for a master controller
 *
 * $Date:        13. May 2020
 * $Revision:    V.1.0.1
 * ----------------------------------------------------------------------
 */


#pragma once
#ifndef MODBUS_MODBUSRTUMASTER_H_
#define MODBUS_MODBUSRTUMASTER_H_
#include <Modbus/Modbus.h>
#include <Modbus/ModbusRtu/ModbusRtu.h>
#include <array>
#include <cstdint>

/*
 * Needs to queue responses
 * */

namespace Modbus {
template <std::size_t kSlaveCount>
class ProtocolRtuMaster : public ProtocolRtu {
  Modbus::Response response_{};
  static const constexpr std::size_t kFrames = 2;
  static const constexpr std::size_t kFrameSize = 256;
  std::array<Modbus::RtuFrame, kFrames> frames_{};
  std::array<std::array<uint8_t, kFrameSize>, kFrames> frame_data_{};
  const std::array<std::size_t, kSlaveCount> slave_addresses_;
  /*
   * Store the last command sent and the past slave address
   * need a state machine with what is being expected
   * */
  enum class State {
    kInitialize,
    kSendingCommand,
    kWaitingForResponse,
  };

 public:
  explicit ProtocolRtuMaster(
      Crc16 crc16, const std::array<std::size_t, kSlaveCount>& slave_addresses)
      : ProtocolRtu{crc16}, slave_addresses_{slave_addresses} {}
};
}  // namespace Modbus

#endif  //  MODBUS_MODBUSRTUMASTER_H_
