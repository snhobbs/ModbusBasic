/* Copyright (C) 2020 Electrooptical Innovations
 * ----------------------------------------------------------------------
 * Project:      Modbus
 * Title:        Modbus/ModbusRtuProtocol.h
 * Description:  Shared functionality between RtuSlaves and RtuMasters
 *
 * $Date:        13. May 2020
 * $Revision:    V.1.0.1
 * ----------------------------------------------------------------------
 */

#pragma once
#include <Modbus/Modbus.h>
#include <Modbus/RegisterControl.h>
#include <Utilities/TypeConversion.h>

#include <cstdint>

#define INTEGER_SIZE sizeof(uint16_t)
namespace Modbus {
using Crc16 = uint16_t (*)(const ArrayView<uint8_t> &, std::size_t);
// using Timer = uint32_t (*)(void); //  Returns the system time in ms

inline constexpr std::size_t GetRequiredPacketSize(
    const Modbus::Frame &packet) {
  return Command::kHeaderLength + packet.data_length + Command::kFooterLength;
}

inline int32_t CalculateMetaDataBytesRemaining(const Function function) {
  if (function == Function::kReadMultipleHoldingRegisters) {
    return ReadMultipleRegistersCommandBase::CommandPacket::kPacketSize;
  } else if (function == Function::kReadInputRegisters) {
    return ReadMultipleRegistersCommandBase::CommandPacket::kPacketSize;
  } else if (function == Function::kWriteSingleHoldingRegister) {
    return WriteSingleHoldingRegisterCommand::CommandPacket::kValueStart;
  } else if (function == Function::kWriteMultipleHoldingRegisters) {
    return WriteMultipleHoldingRegistersCommand::CommandPacket::kValueStart;
  }
  return 0;
  //  Function::kReadExceptionStatus,
  //  Function::kDiagnostic,
  //  Function::kGetComEventCounter,
  //  Function::kGetComEventLog,
  //  Function::kReportSlaveId,
  //  Function::kReadFileRecord,
  //  Function::kWriteFileRecord,
  //  Function::kMaskWriteRegister,
  //  Function::kReadWriteMultipleRegisters,
  //  Function::kReadFifoQueue,
  //  Function::kReadDeviceIdentification,
}

inline int32_t CalculateBytesRemaining(const Frame &frame) {
  const auto function = frame.function;
  std::size_t bytes = 0;
  if (function == Function::kReadMultipleHoldingRegisters) {
    bytes = 0;
  } else if (function == Function::kReadInputRegisters) {
    bytes = 0;
  } else if (function == Function::kWriteSingleHoldingRegister) {
    bytes = sizeof(uint16_t);
  } else if (function == Function::kWriteMultipleHoldingRegisters) {
    bytes =
        frame.data_array
            [WriteMultipleHoldingRegistersCommand::CommandPacket::
                 kNumberOfDataBytes];  // << 8 |
                                       // frame.data_array[WriteMultipleHoldingRegistersCommand::CommandPacket::kNumberOfDataBytes];
  }
  return bytes;
}

class ReadContext {
  PacketState state_ = PacketState::kAddress;
  int32_t bytes_to_read_ = Command::kHeaderLength + 1;

 public:
  PacketState GetState(void) const { return state_; }
  void Reset(void) {
    bytes_to_read_ = Command::kHeaderLength + 1;
    state_ = PacketState::kAddress;
  }

  bool PacketReceived(void) const { return state_ == PacketState::kDone; }

  void ProcessCharacter(Frame *p_frame, const uint8_t pt) {
    bytes_to_read_--;
    switch (state_) {
      case PacketState::kAddress:
        p_frame->address = pt;
        state_ = PacketState::kFunction;
        break;
      case PacketState::kFunction:
        if (FunctionCodeIsValid(pt)) {
          p_frame->function = GetFunction(pt);
          state_ = PacketState::kMeta;
          bytes_to_read_ = CalculateMetaDataBytesRemaining(p_frame->function);
        } else {
          Reset();
        }
        break;
      case PacketState::kMeta:
        //  This contains information on the length or not depending on function
        p_frame->data_array[p_frame->data_length++] = pt;
        if (bytes_to_read_ <= 0) {
          bytes_to_read_ =
              CalculateBytesRemaining(*p_frame) + Command::kFooterLength;
          state_ = PacketState::kData;
          if (bytes_to_read_ == 0) {
            state_ = PacketState::kDone;
          }
        }
        break;

      case PacketState::kData:
        //  Send the data to the function controller, keep reading until it says
        //  its done, character space timeout occurs, or an illegal value is
        //  entered
        p_frame->data_array[p_frame->data_length++] = pt;
        if (bytes_to_read_ <= 0) {
          state_ = PacketState::kDone;
        }
        break;
      case PacketState::kDone:
        assert(0);
        Reset();
        break;
      default:
        assert(0);
        Reset();
    }
  }
};

class ProtocolRtu : public Protocol {
  static const constexpr uint32_t kHeaderLength = Command::kHeaderLength;
  static const constexpr uint32_t kFooterLength = Command::kFooterLength;
  static const constexpr uint32_t kAddressLocation =
      Command::CommandPacket::kSlaveAddress;
  static const constexpr uint32_t kFunctionLocation =
      Command::CommandPacket::kFunction;

 protected:
  const Crc16 crc16_;

 public:
  explicit ProtocolRtu(Crc16 crc16) : crc16_{crc16} {}
  bool FrameCrcIsValid(const Modbus::Frame &frame) const {
    std::array<uint8_t, 256> data{};
    data[kAddressLocation] = frame.address;
    data[kFunctionLocation] = static_cast<uint16_t>(frame.function);
    for (std::size_t i = 0; i < std::min(frame.data_length, data.size()); i++) {
      data[i + kHeaderLength] = frame.data_array[i];
    }
    return FrameCrcIsValid(ArrayView{frame.data_length + 2, data.data()});
  }
  bool FrameCrcIsValid(const ArrayView<uint8_t> &frame_buffer) const {
    const uint16_t frame_crc =
        Utilities::Make_MSB_uint16_tFromU8Array(ArrayView<const uint8_t>{
            sizeof(uint16_t),
            &frame_buffer[frame_buffer.size() - sizeof(uint16_t)]});
    const uint16_t calculated_crc =
        crc16_(frame_buffer, frame_buffer.size() - kFooterLength);
    if (frame_crc != calculated_crc) {
      return false;
    }
    return true;
  }

  int32_t Frame(const Modbus::Frame &packet,
                ArrayView<uint8_t> *frame_buffer) const {
    assert(frame_buffer->size() >= GetRequiredPacketSize(packet));
    frame_buffer->operator[](kAddressLocation) = packet.address;
    frame_buffer->operator[](kFunctionLocation) =
        static_cast<uint16_t>(packet.function);
    //  FIXME use memcopy instead?
    for (std::size_t i = 0; i < packet.data_length; i++) {
      frame_buffer->operator[](i + kHeaderLength) = packet.data_array[i];
    }
    const uint16_t crc =
        crc16_(*frame_buffer, packet.data_length + kHeaderLength);
    assert(GetRequiredPacketSize(packet) == frame_buffer->size());

    const std::size_t crc_start = GetRequiredPacketSize(packet) - sizeof(crc);
    frame_buffer->operator[](crc_start) = Utilities::GetByte(crc, 1);  //  MSB
    frame_buffer->operator[](crc_start + 1) =
        Utilities::GetByte(crc, 0);  //  LSB
    return 0;
  }
  int32_t FrameResponse(const Modbus::Frame &packet,
                        Modbus::Response *response) const {
    response->operator[](Command::ResponsePacket::kSlaveAddress) =
        static_cast<uint8_t>(packet.address);
    assert(response->operator[](Command::ResponsePacket::kSlaveAddress) ==
           static_cast<uint8_t>(packet.address));

    response->operator[](Command::ResponsePacket::kFunction) =
        static_cast<uint8_t>(packet.function);
    assert(response->operator[](Command::ResponsePacket::kFunction) ==
           static_cast<uint8_t>(packet.function));
    const uint16_t crc =
        crc16_(response->GetArrayView(), response->GetLength());
    response->SetLength(response->GetLength() + sizeof(uint16_t));
    const std::size_t crc_start = response->GetLength() - sizeof(uint16_t);

    response->operator[](crc_start) = Utilities::GetByte(crc, 1);      //  LSB
    response->operator[](crc_start + 1) = Utilities::GetByte(crc, 0);  //  MSB
    return 0;
  }

  int32_t ReadFrame(const ArrayView<uint8_t> &frame_buffer,
                    Modbus::Frame *packet) const {
    packet->data_length = frame_buffer.size() - kHeaderLength - kFooterLength;
    assert(packet->data_array.size() >= packet->data_length);
    const uint8_t frame_function = frame_buffer[kFunctionLocation];
    packet->address = frame_buffer[kAddressLocation];
    packet->function = static_cast<Modbus::Function>(frame_function);
    for (std::size_t i = 0; i < packet->data_length; i++) {
      packet->data_array[i] = frame_buffer[i + kHeaderLength];
    }
    return 0;
  }
};
}  // namespace Modbus
