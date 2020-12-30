/* Copyright (C) 2020 Electrooptical Innovations
 * ----------------------------------------------------------------------
 * Project:      Modbus
 * Title:        BitControl.h
 * Description:  Default data controller for the basic bit function
 *               (coils & discrete inputs)
 *
 * $Date:        13. May 2020
 * $Revision:    V.1.0.1
 * ----------------------------------------------------------------------
 */

#pragma once
#include <Modbus/Modbus.h>
#include <Modbus/DataCommand.h>
#include <Utilities/TypeConversion.h>
#include <cstdint>
namespace Modbus {
class CoilCommand : public DataCommand {};

class WriteSingleCoilCommand : public CoilCommand {
 public:
  struct CommandPacket {
    static const constexpr std::size_t kValueStart =
        DataCommand::CommandPacket::kDataAddressEnd + 1;
    static const constexpr std::size_t kPacketSize = kValueStart + sizeof(uint16_t);
  };
  struct ResponsePacket {
    static const std::size_t kDataAddress =
        DataCommand::ResponsePacket::kHeaderEnd + 1;
    static const std::size_t kValue = kDataAddress + sizeof(uint16_t);
    static const std::size_t kPacketSize = kValue + sizeof(uint16_t);
  };
  static Modbus::CoilState ReadState(const ArrayView<uint8_t> &data_array) {
    const uint16_t state =
        Utilities::Make_MSB_uint16_tFromU8Array(ArrayView<const uint8_t>{
            sizeof(uint16_t), &data_array[CommandPacket::kValueStart]});
    if (state == static_cast<uint16_t>(Modbus::CoilState::kOn)) {
      return Modbus::CoilState::kOn;
    }
    return Modbus::CoilState::kOff;
  }
  static uint16_t ReadResponseDataAddress(const Modbus::Response &response) {
    return Utilities::Make_MSB_uint16_tFromU8Array(ArrayView<const uint8_t>{
        sizeof(uint16_t), &response[ResponsePacket::kDataAddress]});
  }
  static uint16_t ReadResponseDataValue(const Modbus::Response &response) {
    return Utilities::Make_MSB_uint16_tFromU8Array(ArrayView<const uint8_t>{
        sizeof(uint16_t), &response[ResponsePacket::kValue]});
  }
  static void FillResponseHeader(const uint8_t slave_address,
                                 const uint16_t address,
                                 const Modbus::CoilState state,
                                 Modbus::Response *response) {
    response->operator[](CoilCommand::ResponsePacket::kSlaveAddress) =
        slave_address;
    response->operator[](CoilCommand::ResponsePacket::kFunction) =
        static_cast<uint8_t>(Modbus::Function::kWriteSingleCoil);
    response->operator[](ResponsePacket::kDataAddress) =
        Utilities::GetByte(address, 1);
    response->operator[](ResponsePacket::kDataAddress + 1) =
        Utilities::GetByte(address, 0);
    response->operator[](ResponsePacket::kValue) =
        Utilities::GetByte(static_cast<uint16_t>(state), 1);
    response->operator[](ResponsePacket::kValue + 1) =
        Utilities::GetByte(static_cast<uint16_t>(state), 0);

    response->SetLength(ResponsePacket::kPacketSize);
  }
  static int32_t FillFrame(const uint16_t starting_address,
                           const Modbus::CoilState state, Modbus::Frame *frame) {
    assert(frame->data_array.size() >= CommandPacket::kPacketSize);

    frame->function = Modbus::Function::kWriteSingleCoil;
    frame->data_array[DataCommand::CommandPacket::kDataAddressStart] =
        Utilities::GetByte(starting_address, 1);
    frame->data_array[DataCommand::CommandPacket::kDataAddressStart + 1] =
        Utilities::GetByte(starting_address, 0);
    frame->data_array[CommandPacket::kValueStart] =
        Utilities::GetByte(static_cast<uint16_t>(state), 1);
    frame->data_array[CommandPacket::kValueStart + 1] =
        Utilities::GetByte(static_cast<uint16_t>(state), 0);
    frame->data_length = CommandPacket::kPacketSize;
    return 0;
  }
};

class WriteMultipleCoilsCommand : public CoilCommand {
 public:
  struct CommandPacket {
    static const constexpr std::size_t kCoilCount =
        DataCommand::CommandPacket::kDataAddressEnd + 1;
    static const constexpr std::size_t kNumberOfDataBytes =
        kCoilCount + sizeof(uint16_t);
    static const constexpr std::size_t kHeaderSize =
    		kNumberOfDataBytes + 1;
    // FIXME static_assert(kHeaderSize == 6);
    static const constexpr std::size_t kValueStart =
    		kHeaderSize;
  };
  struct ResponsePacket {
    static const constexpr std::size_t kDataAddress =
        DataCommand::ResponsePacket::kHeaderEnd + 1;
    static const constexpr std::size_t kCoilCount = kDataAddress + sizeof(uint16_t);
    static const constexpr std::size_t kPacketSize = kCoilCount + sizeof(uint16_t);
  };
  static uint16_t ReadCommandCoilCount(const ArrayView<uint8_t> &data_array) {
    return Utilities::Make_MSB_uint16_tFromU8Array(ArrayView<const uint8_t>{
        sizeof(uint16_t), &data_array[CommandPacket::kCoilCount]});
  }
  static uint16_t ReadCommandDataByteCount(const ArrayView<uint8_t> &data_array) {
	  return data_array[CommandPacket::kNumberOfDataBytes];
  }
  static uint16_t ReadResponseCoilCount(const Modbus::Response &data_array) {
    return Utilities::Make_MSB_uint16_tFromU8Array(ArrayView<const uint8_t>{
        sizeof(uint16_t), &data_array[ResponsePacket::kCoilCount]});
  }
  static uint16_t ReadResponseDataAddressStart(
      const Modbus::Response &data_array) {
    return Utilities::Make_MSB_uint16_tFromU8Array(ArrayView<const uint8_t>{
        sizeof(uint16_t), &data_array[ResponsePacket::kDataAddress]});
  }

  static void FillResponseHeader(const uint8_t slave_address,
                                 const uint16_t starting_address,
                                 const uint16_t coil_count,
                                 Modbus::Response *response) {
    response->operator[](DataCommand::ResponsePacket::kSlaveAddress) =
        slave_address;
    response->operator[](DataCommand::ResponsePacket::kFunction) =
        static_cast<uint8_t>(Modbus::Function::kWriteMultipleCoils);
    response->operator[](ResponsePacket::kDataAddress) =
        Utilities::GetByte(starting_address, 1);
    response->operator[](ResponsePacket::kDataAddress + 1) =
        Utilities::GetByte(starting_address, 0);
    response->operator[](ResponsePacket::kCoilCount) =
        Utilities::GetByte(coil_count, 1);
    response->operator[](ResponsePacket::kCoilCount + 1) =
        Utilities::GetByte(coil_count, 0);
    response->SetLength(ResponsePacket::kPacketSize);
  }
  static int32_t FillFrame(const uint16_t starting_address,
                           const uint16_t coil_count,
                           const ArrayView<const uint8_t> &data,
                           Modbus::Frame *frame) {
    const std::size_t frame_size = CommandPacket::kHeaderSize + data.size();
    assert(frame->data_array.size() >= frame_size);

    frame->function = Modbus::Function::kWriteMultipleCoils;
    frame->data_array[CommandPacket::kCoilCount] =
        Utilities::GetByte(coil_count, 1);
    frame->data_array[CommandPacket::kCoilCount + 1] =
        Utilities::GetByte(coil_count, 0);
    frame->data_array[DataCommand::CommandPacket::kDataAddressStart] =
        Utilities::GetByte(starting_address, 1);
    frame->data_array[DataCommand::CommandPacket::kDataAddressStart + 1] =
        Utilities::GetByte(starting_address, 0);
    frame->data_array[CommandPacket::kNumberOfDataBytes] = data.size();
    std::size_t index = 0;
    for (auto byte : data) {
      frame->data_array[CommandPacket::kValueStart + index++] = byte;
    }
    frame->data_length = frame_size;
    return 0;
  }
};

class ReadBitsCommand : public CoilCommand {
 public:
  struct CommandPacket {
    static const constexpr std::size_t kNumberOfElementsStart =
        DataCommand::CommandPacket::kDataAddressEnd + 1;
    static const constexpr std::size_t kNumberOfElementsEnd =
        kNumberOfElementsStart + sizeof(uint16_t) - 1;
    static const constexpr std::size_t kPacketSize = kNumberOfElementsEnd + 1;
  };
  struct ResponsePacket {
    static const std::size_t kNumDataBytes =
        DataCommand::ResponsePacket::kHeaderEnd + 1;
    static const std::size_t kHeaderEnd = kNumDataBytes;
    static const std::size_t kHeaderSize = kHeaderEnd + 1;
  };

  static void FillResponseHeader(const uint16_t element_count,
                                     Modbus::Response *response) {
    response->operator[](ResponsePacket::kNumDataBytes) =
        Utilities::CalcNumberOfBytesToFitBits(element_count);
    response->SetLength(Utilities::CalcNumberOfBytesToFitBits(element_count) +
                        ResponsePacket::kHeaderSize);
  }
  static uint16_t ReadCommandElementCount(
      const ArrayView<uint8_t> &data_array) {
    return Utilities::Make_MSB_uint16_tFromU8Array(ArrayView<const uint8_t>{
        sizeof(uint16_t), &data_array[CommandPacket::kNumberOfElementsStart]});
  }
  static uint16_t ReadResponseNumberOfDataBytes(
      const ArrayView<uint8_t> &data_array) {
    return Utilities::Make_MSB_uint16_tFromU8Array(ArrayView<const uint8_t>{
        sizeof(uint16_t), &data_array[ResponsePacket::kNumDataBytes]});
  }
  static int32_t FillFrame(const uint16_t starting_address,
                               const uint16_t element_count, Modbus::Frame *frame) {
    frame->data_array[CommandPacket::kNumberOfElementsStart] =
        Utilities::GetByte(element_count, 1);
    frame->data_array[CommandPacket::kNumberOfElementsStart + 1] =
        Utilities::GetByte(element_count, 0);
    frame->data_array[DataCommand::CommandPacket::kDataAddressStart] =
        Utilities::GetByte(starting_address, 1);
    frame->data_array[DataCommand::CommandPacket::kDataAddressStart + 1] =
        Utilities::GetByte(starting_address, 0);
    frame->data_length = CommandPacket::kPacketSize;
    return 0;
  }
};

template <typename T>
class BitController {
 protected:
  T byte_field_controller_;

  Modbus::Exception ValidateReadMultipleBits(
      const ArrayView<uint8_t> &data_array) const {
    const std::size_t address = ReadBitsCommand::ReadAddressStart(data_array);
    const std::size_t element_count = ReadBitsCommand::ReadCommandElementCount(
        data_array);  // fixme factor out and rename
    if (!(IsAddressValid(address) &&
          IsAddressValid(address + element_count - 1))) {
      return Modbus::Exception::kIllegalDataAddress;
    }
    return Modbus::Exception::kAck;
  }
 public:
  static std::size_t size(void) {return T::size();}
  void ReadBits(const uint16_t starting_address,
                        const uint16_t element_count,
                        ArrayView<uint8_t> *response_data) {
    assert(IsAddressValid(starting_address + element_count - 1));
    byte_field_controller_.ReadElementsToBytes(starting_address, element_count,
                                               response_data);
  }

  //static uint16_t GetBitCount(void) { return kSize; }
  bool IsAddressValid(uint16_t address) const {
    return byte_field_controller_.IsAddressValid(address);
  }
  int32_t WriteElement(uint16_t address, bool state) {
    byte_field_controller_.WriteElement(address, state);
    return 0;
  }

  bool ReadElement(uint16_t address) const {
    return byte_field_controller_.ReadElement(address);
  }

  int32_t RunReadBits(const Modbus::Frame &frame, Modbus::Response *response) {
    const uint16_t address = CoilCommand::ReadAddressStart(frame.data_array);

    const uint16_t count =
        ReadBitsCommand::ReadCommandElementCount(frame.data_array);
    auto response_data = ArrayView<uint8_t>{
        response->size() - ReadBitsCommand::ResponsePacket::kHeaderEnd + 1,
        &response->data()[ReadBitsCommand::ResponsePacket::kHeaderEnd + 1]};
    ReadBits(address, count, &response_data);
    ReadBitsCommand::FillResponseHeader(count, response);
    return 0;
  }
};

template <typename T>
class CoilController : public BitController<T>{
  Modbus::Exception ValidateWriteSingleCoil(
      const ArrayView<uint8_t> &data_array) const {
    const std::size_t address =
        WriteSingleCoilCommand::ReadAddressStart(data_array);
    if (!BitController<T>::IsAddressValid(address)) {
      return Modbus::Exception::kIllegalDataAddress;
    }
    return Modbus::Exception::kAck;
  }
  Modbus::Exception ValidateReadCoils(
      const ArrayView<uint8_t> &data_array) const {
    const std::size_t address = ReadBitsCommand::ReadAddressStart(data_array);
    const std::size_t coil_count = ReadBitsCommand::ReadCommandElementCount(data_array);
    if (!(BitController<T>::IsAddressValid(address) &&
          BitController<T>::IsAddressValid(address + coil_count - 1))) {
      return Modbus::Exception::kIllegalDataAddress;
    }
    return Modbus::Exception::kAck;
  }
  Modbus::Exception ValidateWriteMultipleCoils(
      const ArrayView<uint8_t> &data_array) const {
    Modbus::Exception exception = ValidateReadCoils(data_array);
    if (exception == Modbus::Exception::kAck) {
      const uint16_t coil_count =
          WriteMultipleCoilsCommand::ReadCommandCoilCount(data_array);
      const std::size_t num_data_bytes =
          WriteMultipleCoilsCommand::ReadCommandDataByteCount(data_array);
      if (Utilities::CalcNumberOfBytesToFitBits(coil_count) != num_data_bytes) {
        exception = Modbus::Exception::kIllegalDataValue;
      }
    }
    return exception;
  }

 public:
  Modbus::Exception ValidateFrame(const Modbus::Frame &frame) const {
    Modbus::Exception exception = Modbus::Exception::kAck;
    if (frame.function == Modbus::Function::kReadCoils) {
      exception = ValidateReadCoils(frame.data_array);
    } else if (frame.function == Modbus::Function::kWriteSingleCoil) {
      exception = ValidateWriteSingleCoil(frame.data_array);
    } else if (frame.function == Modbus::Function::kWriteMultipleCoils) {
      exception = ValidateWriteMultipleCoils(frame.data_array);
    } else {
      assert(0);
    }
    return exception;
  }
  void WriteMultipleCoils(const uint16_t starting_address,
                          const uint16_t coil_count,
                          const uint16_t num_data_bytes,
                          const ArrayView<const uint8_t> data) {
    assert(Utilities::CalcNumberOfBytesToFitBits(coil_count) == num_data_bytes);
    assert(data.size() >= num_data_bytes);
    BitController<T>::byte_field_controller_.WriteMultipleElementsFromBytes(starting_address, coil_count,
                                              data);
  }

  Modbus::CoilState ReadCoil(uint16_t address) const {
    if (BitController<T>::ReadElement(address)) {
      return Modbus::CoilState::kOn;
    }
    return Modbus::CoilState::kOff;
  }
  int32_t ReadFrame(const Modbus::Frame &frame, Modbus::Response *response) {
    switch (frame.function) {
      case (Modbus::Function::kReadCoils):
        BitController<T>::RunReadBits(frame, response);
        break;
      case (Modbus::Function::kWriteSingleCoil):
        return RunWriteSingleCoil(frame, response);
        break;
      case (Modbus::Function::kWriteMultipleCoils):
        return RunWriteMultipleCoils(frame, response);
        break;
      default:
        return -1;
    }
    return 0;
  }

  int32_t RunWriteSingleCoil(const Modbus::Frame &frame,
                             Modbus::Response *response) {
    const uint16_t address = CoilCommand::ReadAddressStart(frame.data_array);
    const Modbus::CoilState state =
        WriteSingleCoilCommand::ReadState(frame.data_array);
    BitController<T>::WriteElement(address, state == Modbus::CoilState::kOn);
    WriteSingleCoilCommand::FillResponseHeader(frame.address, address, state,
                                               response);
    return 0;
  }

  int32_t RunWriteMultipleCoils(const Modbus::Frame &frame,
                                Modbus::Response *response) {
    const uint16_t address = CoilCommand::ReadAddressStart(frame.data_array);
    const uint16_t coil_count =
        WriteMultipleCoilsCommand::ReadCommandCoilCount(frame.data_array);
    const uint16_t num_data_bytes =
        WriteMultipleCoilsCommand::ReadCommandDataByteCount(frame.data_array);
    const ArrayView<const uint8_t> array_view{
        frame.data_array.size() -
            WriteMultipleCoilsCommand::CommandPacket::kValueStart,
        &frame.data_array
             .data()[WriteMultipleCoilsCommand::CommandPacket::kValueStart]};
    WriteMultipleCoils(address, coil_count, num_data_bytes, array_view);
    WriteMultipleCoilsCommand::FillResponseHeader(frame.address, address,
                                                  coil_count, response);
    return 0;
  }

  CoilController() {}
};


template <typename T>
class DiscreteInputController : public BitController<T> {
 public:
  Modbus::Exception ValidateFrame(const Modbus::Frame &frame) const {
    Modbus::Exception exception = Modbus::Exception::kAck;
    if (frame.function == Modbus::Function::kReadDiscreteInputs) {
      exception = BitController<T>::ValidateReadMultipleBits(frame.data_array);
    } else {
      assert(0);
    }
    return exception;
  }
  int32_t ReadFrame(const Modbus::Frame &frame, Modbus::Response *response) {
    if (frame.function == Modbus::Function::kReadDiscreteInputs) {
      return BitController<T>::RunReadBits(frame, response);
    } else {
      assert(0);
      return -1;
    }
  }

  DiscreteInputController() {}
};
}  //  namespace Modbus
