/* Copyright (C) 2020 Electrooptical Innovations
 * ----------------------------------------------------------------------
 * Project:      Modbus
 * Title:        ModbusRtu/ModbusRtuSlave.h
 * Description:  Controller for a rtu slave, this is a template that
 *               is the base class of the final slave
 *
 * $Date:        13. May 2020
 * $Revision:    V.1.0.1
 *
 * ----------------------------------------------------------------------
 *
 * How to deliminate packets coming in -> this is on another level
 * Resyncronizing packets with timeouts -> Clear the buffer when a timer times
 * out, reset the timer when a character comes in
 */

#pragma once
#ifndef MODBUS_MODBUSRTUSLAVE_H_
#define MODBUS_MODBUSRTUSLAVE_H_
#include <Modbus/Modbus.h>
#include <Modbus/ModbusRtu/ModbusRtuProtocol.h>
#include <Modbus/Utilities.h>
#include <Utilities/TypeConversion.h>

#include <cstdint>
namespace Modbus {

/*
 * Slave base should be rx packet reader
 * */
class SlaveProtocolBase : public ProtocolRtu {
 private:
  Response response_{};
  bool response_valid_ = false;
  std::array<uint8_t, 256> frame_data_{};
  Modbus::Frame framein_{ArrayView{frame_data_.size(), frame_data_.data()}};

 public:
  ReadContext ctx_{};

  void ProcessCharacter(const uint8_t pt) {
    ctx_.ProcessCharacter(&framein_, pt);
  }
  void SetResponse(const Response& response) {
    response_.Reset();
    response_ = response;
  }

  void SendErrorResponse(const Modbus::Frame& frame, Exception exception) {
    Modbus::Frame error_frame;
    error_frame.address = frame.address;
    error_frame.function = GetErrorFunction(frame.function);
    response_.Reset();
    response_[Command::ResponsePacket::kHeaderEnd + 1] =
        static_cast<uint8_t>(exception);
    response_.SetLength(Command::ResponsePacket::kHeaderEnd + 2);
    FrameResponse(error_frame, &response_);
    SetResponseValid(true);
  }

  const Modbus::Frame& GetFrameIn(void) { return framein_; }

  void ResetRead(void) {
    ctx_.Reset();
    ResetResponse();
    response_.Reset();
    framein_.Reset();
  }

  const Response& GetResponse(void) { return response_; }

  void ResetResponse(void) {
    SetResponseValid(false);
    response_.Reset();
  }

  bool GetResponseValid(void) const { return response_valid_; }
  void SetResponseValid(bool on) { response_valid_ = on; }
  explicit SlaveProtocolBase(Crc16 crc16) : ProtocolRtu{crc16} {}
};

template <typename THoldingRegisterController,
          typename TInputRegisterController>
class ProtocolRtuSlave {
 protected:
  static const constexpr Function implimented_functions_[]{
      //  Function::kReadCoils,
      //  Function::kReadDiscreteInputs,
      Function::kReadMultipleHoldingRegisters, Function::kReadInputRegisters,
      //  Function::kWriteSingleCoil,
      Function::kWriteSingleHoldingRegister,
      Function::kWriteMultipleHoldingRegisters,
      //  Function::kWriteMultipleCoils,
      //  Function::kReadDeviceIdentification,
  };

  SlaveProtocolBase slave_{};
  uint8_t slave_address_;
  THoldingRegisterController& holding_register_controller_;
  TInputRegisterController& input_register_controller_;

 public:
  /*
   * Check the command for basic errors in function, data address, or data valid
   * */
  /*
   * Each command needs to queue a response when it completes
   * Some will have data, some just the same command acked
   * some with error status included
   * all commands have the response of slave address, function, {payload
   * specific response}, crc lsb, crc msb
   * */
  int32_t RunCommand(const Modbus::Frame& frame) {
    int32_t response_code = 0;
    Response response{};
    switch (GetAddressSpaceFromFunction(frame.function)) {
      case (AddressSpace::kCoil):
        response_code = -1;
        break;
      case (AddressSpace::kDiscreteInput):
        response_code = -1;
        break;
      case (AddressSpace::kInputRegister):
        response_code = input_register_controller_.ReadFrame(frame, &response);
        break;
      case (AddressSpace::kHoldingRegister):
        response_code =
            holding_register_controller_.ReadFrame(frame, &response);
        break;
      case (AddressSpace::kDeviceIdentifier):
        response_code = -1;
        break;
      case (AddressSpace::kUnmapped):
        response_code = -1;
        break;
      default:
        response_code = -1;
        break;
    }
    if (response_code == 0) {
      slave_.FrameResponse(frame, &response);
      slave_.SetResponseValid(true);
    }
    slave_.SetResponse(response);
    return response_code;
  }

  const Modbus::Frame& GetFrameIn(void) { return slave_.GetFrameIn(); }
  const Response& GetResponse(void) { return slave_.GetResponse(); }
  bool GetResponseValid(void) { return slave_.GetResponseValid(); }

  void Reset(void) {
    slave_.ResetResponse();
    slave_.ResetRead();
  }

  /*
   * Check the command for basic errors in function, data address, or data valid
   * */
  Exception ValidateMessage(const Modbus::Frame& frame) const {
    if (!InList(frame.function, implimented_functions_,
                sizeof(implimented_functions_) /
                    sizeof(implimented_functions_[0]))) {
      return Exception::kIllegalFunction;
    }
    switch (GetAddressSpaceFromFunction(frame.function)) {
      case (AddressSpace::kCoil):
        return Exception::kIllegalFunction;
      case (AddressSpace::kDiscreteInput):
        return Exception::kIllegalFunction;
      case (AddressSpace::kInputRegister):
        return input_register_controller_.ValidateFrame(frame);
      case (AddressSpace::kHoldingRegister):
        return holding_register_controller_.ValidateFrame(frame);
      case (AddressSpace::kDeviceIdentifier):
        return Exception::kIllegalFunction;
      case (AddressSpace::kSystemStatus):
        return Exception::kIllegalFunction;
      case (AddressSpace::kUnmapped):
        return Exception::kIllegalFunction;
      default:
        break;
    }
    return Exception::kIllegalFunction;
  }

  uint8_t GetAddress(void) const { return slave_address_; }
  void SetAddress(const uint8_t slave_address) {
    slave_address_ = slave_address;
  }

  void ProcessMessageNoAddress(void) { ProcessMessage(false); }

  const ReadContext& GetContext(void) const { return slave_.ctx_; }

  void ProcessMessage(const bool filter_address = true) {
    const auto& frame = GetFrameIn();
    Exception exception = ValidateMessage(frame);
    if (GetAddress() == frame.address || !filter_address) {
      if (slave_.FrameCrcIsValid(frame)) {
        if (exception == Exception::kAck) {
          RunCommand(frame);
        } else {
          slave_.SendErrorResponse(frame, exception);
        }
      }
    }
  }

  explicit ProtocolRtuSlave(
      Crc16 crc16, uint8_t slave_address,
      THoldingRegisterController& holding_register_controller,
      TInputRegisterController& input_register_controller)
      : slave_{crc16},
        slave_address_{slave_address},
        holding_register_controller_{holding_register_controller},
        input_register_controller_{input_register_controller} {}
};
}  //  namespace Modbus

#endif  //  MODBUS_MODBUSRTUSLAVE_H_
