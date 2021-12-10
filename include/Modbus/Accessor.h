#include "Modbus/BitController.h"
#include "Modbus/RegisterController.h"
#include <cstddef>
#include <cstdint>

/*
 * All protocol requests are passed to the accesor
 * Can preset the underlying sections as jointed or disjointed
 * */

enum class FieldType {
  kCoil,
  kDiscrete,
  kHoldingRegister,
  kInputRegister
};

class Accessor {
  BitController* p_coil_;
  BitController* p_discrete_input_;
  RegisterController* p_holding_register_;
  RegisterController* p_input_register_;

 public:
  bool access_valid(const size_t address, const size_t length, const FieldType field_type) const {
    bool valid = false;
    switch (field_type) {
    case FieldType::kCoil:
      valid = p_coil_->access_valid(address, length);
      break;
    case FieldType::kDiscrete:
      valid = p_discrete_input_->access_valid(address, length);
      break;
    case FieldType::kHoldingRegister:
      valid = p_holding_register_->access_valid(address, length);
      break;
    case FieldType::kInputRegister:
      valid = p_input_register_->access_valid(address, length);
      break;
    default:
      valid = false;
      break;
    }
    return valid;
  }

  size_t read(const size_t address, const size_t length, const FieldType field_type, u8_Buffer* buffer) const {
    size_t entries_read = 0;
    switch (field_type) {
    case FieldType::kCoil:
      entries_read = p_coil_->read(address, length, buffer);
      break;
    case FieldType::kDiscrete:
      entries_read = p_discrete_input_->read(address, length, buffer);
      break;
    case FieldType::kHoldingRegister:
      entries_read = p_holding_register_->read(address, length, buffer);
      break;
    case FieldType::kInputRegister:
      entries_read = p_input_register_->read(address, length, buffer);
      break;
    }
    return entries_read;
  }

  size_t write(const size_t address, const size_t length, const FieldType field_type, const u8_Buffer* buffer) {
    size_t entries_write = 0;
    switch (field_type) {
    case FieldType::kCoil:
      entries_write = p_coil_->write(address, length, buffer);
      break;
    case FieldType::kDiscrete:
      entries_write = p_discrete_input_->write(address, length, buffer);
      break;
    case FieldType::kHoldingRegister:
      entries_write = p_holding_register_->write(address, length, buffer);
      break;
    case FieldType::kInputRegister:
      entries_write = p_input_register_->write(address, length, buffer);
      break;
    }
    return entries_write;
  }

  Accessor(BitController* p_coil_controller, BitController* p_discrete_input_controller,
      RegisterController* p_holding_register_controller,
      RegisterController* p_input_register_controller) : p_coil_{p_coil_controller}, p_discrete_input_{p_discrete_input_controller},
p_holding_register_{p_holding_register_controller}, p_input_register_{p_input_register_controller} {}
};
