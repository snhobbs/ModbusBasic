/*
 * Copyright 2020 ElectroOptical Innovations
 * ConnectedSystem.h
 *
 *  Created on: Jan 18, 2020
 *      Author: simon
 */
#pragma once
#if 0
#define CONNECTEDSYSTEM_H_
#ifndef CONNECTEDSYSTEM_H_
#define CONNECTEDSYSTEM_H_
#include <ArrayView/ArrayView.h>
#include <IODevice/IODevice.h>
#include <Modbus/ModbusRtu/ModbusRtu.h>
#include <Modbus/ModbusRtu/ModbusRtuMaster.h>
#include <Modbus/ModbusRtu/ModbusRtuSlave.h>
#include <RingBuffer/RingBuffer.h>

#include <array>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <vector>

#include "Crc.h"
#include "SlaveDeviceIdentifier.h"

class ConnectedSystem {
  static const constexpr SlaveDeviceIdentifier identity{};
 public:
  static const constexpr int kFrameOverhead =
      Modbus::RtuPacket::kHeaderLength + Modbus::RtuPacket::kFooterLength;
  static const constexpr std::size_t kSlaveAddress = 0xfa;
  static const constexpr std::size_t coil_count = 64;
  static const constexpr std::size_t holding_register_count = 64;
  static const constexpr std::size_t kSlaveCount = 1;
  static const constexpr std::array<std::size_t, kSlaveCount> slave_addresses{
      kSlaveAddress};
  Modbus::ProtocolRtuSlave<coil_count, holding_register_count> slave{
      &crc16, slave_addresses[0], identity.GetDeviceIdentifier()};
  Modbus::ProtocolRtuMaster<kSlaveCount> master{&crc16, slave_addresses};

  RingBuffer<uint8_t, 256> master_rx;
  RingBuffer<uint8_t, 256> master_tx;

  IODevice master_dev{master_rx, master_tx};
  IODevice slave_dev{master_tx, master_rx};

  void Setup(void) {}

  void RunThrough(void) {
    for (int i = 0; i < 1000; i++) {
    }
  }

  ConnectedSystem(void) { Setup(); }
};

#endif /* CONNECTEDSYSTEM_H_ */
#endif
