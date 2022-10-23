#pragma once

#include <cstddef>
#include <cstdint>

#include "Modbus/Modbus.h"
#include "Modbus/RegisterController.h"

/*
 * All protocol requests are passed to the accesor
 * Can preset the underlying sections as jointed or disjointed
 * */
namespace Modbus {
class Accessor {
  RegisterController* p_holding_register_;
  RegisterController* p_input_register_;

 public:
  bool access_valid(const size_t address, const size_t length,
                    const AddressSpace address_space) const {
    bool valid = false;
    switch (address_space) {
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

  size_t read(const size_t address, const size_t length,
              const AddressSpace address_space, u8_Buffer* buffer) const {
    size_t entries_read = 0;
    switch (address_space) {
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

  size_t write(const size_t address, const size_t length,
               const AddressSpace address_space, const u8_Buffer* buffer) {
    size_t entries_write = 0;
    switch (address_space) {
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

  Accessor(RegisterController* p_holding_register_controller,
           RegisterController* p_input_register_controller)
      : p_holding_register_{p_holding_register_controller},
        p_input_register_{p_input_register_controller} {}
};
}  // namespace Modbus
