#pragma once

#include "Modbus/Modbus.h"
#include "Modbus/BitController.h"
#include "Modbus/RegisterController.h"
#include <cstddef>
#include <cstdint>

/*
 * All protocol requests are passed to the accesor
 * Can preset the underlying sections as jointed or disjointed
 * */
namespace Modbus {
class Accessor {
  BitController* p_coil_;
  BitController* p_discrete_input_;
  RegisterController* p_holding_register_;
  RegisterController* p_input_register_;

 public:
  bool access_valid(const size_t address, const size_t length, const AddressSpace address_space) const {
    bool valid = false;
    switch (address_space) {
    case AddressSpace::kCoil:
      valid = p_coil_->access_valid(address, length);
      break;
    case AddressSpace::kDiscreteInput:
      valid = p_discrete_input_->access_valid(address, length);
      break;
    case AddressSpace::kHoldingRegister:
      valid = p_holding_register_->access_valid(address, length);
      break;
    case AddressSpace::kInputRegister:
      valid = p_input_register_->access_valid(address, length);
      break;
    default:
      valid = false;
      break;
    }
    return valid;
  }

  size_t read(const size_t address, const size_t length, const AddressSpace address_space, u8_Buffer* buffer) const {
    size_t entries_read = 0;
    switch (address_space) {
    case AddressSpace::kCoil:
      entries_read = p_coil_->read(address, length, buffer);
      break;
    case AddressSpace::kDiscreteInput:
      entries_read = p_discrete_input_->read(address, length, buffer);
      break;
    case AddressSpace::kHoldingRegister:
      entries_read = p_holding_register_->read(address, length, buffer);
      break;
    case AddressSpace::kInputRegister:
      entries_read = p_input_register_->read(address, length, buffer);
      break;
    default:
      break;
    }
    return entries_read;
  }

  size_t write(const size_t address, const size_t length, const AddressSpace address_space, const u8_Buffer* buffer) {
    size_t entries_write = 0;
    switch (address_space) {
    case AddressSpace::kCoil:
      entries_write = p_coil_->write(address, length, buffer);
      break;
    case AddressSpace::kDiscreteInput:
      entries_write = p_discrete_input_->write(address, length, buffer);
      break;
    case AddressSpace::kHoldingRegister:
      entries_write = p_holding_register_->write(address, length, buffer);
      break;
    case AddressSpace::kInputRegister:
      entries_write = p_input_register_->write(address, length, buffer);
      break;
    default:
      break;
    }
    return entries_write;
  }

  Accessor(BitController* p_coil_controller, BitController* p_discrete_input_controller,
      RegisterController* p_holding_register_controller,
      RegisterController* p_input_register_controller) : p_coil_{p_coil_controller}, p_discrete_input_{p_discrete_input_controller},
p_holding_register_{p_holding_register_controller}, p_input_register_{p_input_register_controller} {}
};
}
