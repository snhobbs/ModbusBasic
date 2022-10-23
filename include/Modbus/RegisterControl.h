/* Copyright (C) 2020 Electrooptical Innovations
 * ----------------------------------------------------------------------
 * Project:      Modbus
 * Title:        RegisterControl.h
 * Description:  Default data controller for the basic register function
 *               (holding & input registers)
 *
 * $Date:        13. May 2020
 * $Revision:    V.1.0.1
 *
 * Target Processor: Any, Linux system
 * ----------------------------------------------------------------------
 */

#pragma once
#include <Modbus/DataCommand.h>
#include <Modbus/Modbus.h>

#include <algorithm>

namespace Modbus {
class RegisterCommand : public DataCommand {};

class WriteSingleHoldingRegisterCommand : public RegisterCommand {
 public:
  static const constexpr auto kFunction = Function::kWriteSingleHoldingRegister;
  struct CommandPacket {
    static const constexpr std::size_t kValueStart =
        DataCommand::CommandPacket::kDataAddressEnd + 1;
    static const constexpr std::size_t kPacketSize =
        kValueStart + sizeof(uint16_t);
  };
  struct ResponsePacket {
    static const constexpr std::size_t kDataAddress =
        DataCommand::ResponsePacket::kHeaderEnd + 1;
    static const constexpr std::size_t kValueStart =
        kDataAddress + sizeof(uint16_t);
    static const constexpr std::size_t kPacketSize =
        kValueStart + sizeof(uint16_t);
  };
  static uint16_t ReadSetting(const ArrayView<uint8_t> &data_array) {
    return Utilities::Make_MSB_uint16_tFromU8Array(ArrayView<const uint8_t>{
        sizeof(uint16_t), &data_array[CommandPacket::kValueStart]});
  }
  static uint16_t ReadResponseSetting(const ArrayView<uint8_t> &data_array) {
    return Utilities::Make_MSB_uint16_tFromU8Array(ArrayView<const uint8_t>{
        sizeof(uint16_t), &data_array[ResponsePacket::kValueStart]});
  }
  static uint16_t ReadResponseAddressStart(
      const ArrayView<uint8_t> &data_array) {
    return Utilities::Make_MSB_uint16_tFromU8Array(ArrayView<const uint8_t>{
        sizeof(uint16_t), &data_array[ResponsePacket::kDataAddress]});
  }
  static void FillResponseHeader(const uint8_t slave_address,
                                 const uint16_t address, const uint16_t value,
                                 Response *response) {
    response->operator[](DataCommand::ResponsePacket::kSlaveAddress) =
        slave_address;
    response->operator[](DataCommand::ResponsePacket::kFunction) =
        static_cast<uint8_t>(kFunction);
    response->operator[](ResponsePacket::kDataAddress) =
        Utilities::GetByte(address, 1);
    response->operator[](ResponsePacket::kDataAddress + 1) =
        Utilities::GetByte(address, 0);
    response->operator[](ResponsePacket::kValueStart) =
        Utilities::GetByte(value, 1);
    response->operator[](ResponsePacket::kValueStart + 1) =
        Utilities::GetByte(value, 0);

    response->SetLength(ResponsePacket::kPacketSize);
  }
  static int32_t FillFrame(const uint16_t address, const uint16_t value,
                           Frame *frame) {
    const std::size_t frame_size = CommandPacket::kPacketSize;
    assert(frame->data_array.size() >= frame_size);

    frame->function = kFunction;
    frame->data_array[DataCommand::CommandPacket::kDataAddressStart] =
        Utilities::GetByte(address, 1);
    frame->data_array[DataCommand::CommandPacket::kDataAddressStart + 1] =
        Utilities::GetByte(address, 0);
    frame->data_array[CommandPacket::kValueStart] =
        Utilities::GetByte(value, 1);
    frame->data_array[CommandPacket::kValueStart + 1] =
        Utilities::GetByte(value, 0);
    frame->data_length = frame_size;
    return 0;
  }
};
class ReadMultipleRegistersCommandBase : public RegisterCommand {
 public:
  struct CommandPacket {
    static const constexpr std::size_t kRegisterCount =
        DataCommand::CommandPacket::kDataAddressEnd + 1;
    static const constexpr std::size_t kPacketSize =
        kRegisterCount + sizeof(uint16_t);
  };
  struct ResponsePacket {
    static const std::size_t kNumberOfBytes =
        Command::ResponsePacket::kHeaderEnd + 1;
    static const constexpr std::size_t kHeaderSize = kNumberOfBytes + 1;
  };
  static uint16_t ReadRegisterCount(const ArrayView<uint8_t> &data_array) {
    return Utilities::Make_MSB_uint16_tFromU8Array(ArrayView<const uint8_t>{
        sizeof(uint16_t), &data_array[CommandPacket::kRegisterCount]});
  }
  static uint8_t ReadResponseByteCount(const ArrayView<uint8_t> &data_array) {
    return data_array[ResponsePacket::kNumberOfBytes];
  }

 protected:
  static void FillResponseHeaderBase(const uint8_t slave_address,
                                     const std::size_t register_count,
                                     Response *response, Function function) {
    response->operator[](DataCommand::ResponsePacket::kSlaveAddress) =
        slave_address;
    response->operator[](DataCommand::ResponsePacket::kFunction) =
        static_cast<uint8_t>(function);
    const std::size_t number_of_data_bytes = register_count * sizeof(uint16_t);
    response->operator[](ResponsePacket::kNumberOfBytes) =
        static_cast<uint8_t>(number_of_data_bytes);
    response->SetLength(ResponsePacket::kHeaderSize + number_of_data_bytes);
  }
  static int32_t FillFrameBase(const uint16_t address, const uint16_t value,
                               Frame *frame, Function function) {
    assert(frame->data_array.size() >= CommandPacket::kPacketSize);
    frame->function = function;
    frame->data_array[DataCommand::CommandPacket::kDataAddressStart] =
        Utilities::GetByte(address, 1);
    frame->data_array[DataCommand::CommandPacket::kDataAddressStart + 1] =
        Utilities::GetByte(address, 0);
    frame->data_array[CommandPacket::kRegisterCount] =
        Utilities::GetByte(value, 1);
    frame->data_array[CommandPacket::kRegisterCount + 1] =
        Utilities::GetByte(value, 0);
    frame->data_length = CommandPacket::kPacketSize;
    return 0;
  }
};

class ReadMultipleHoldingRegistersCommand
    : public ReadMultipleRegistersCommandBase {
  static const constexpr Function kFunction =
      Function::kReadMultipleHoldingRegisters;

 public:
  static int32_t FillFrame(const uint16_t address, const uint16_t value,
                           Frame *frame) {
    return FillFrameBase(address, value, frame, kFunction);
  }

  static void FillResponseHeader(const uint8_t slave_address,
                                 const std::size_t register_count,
                                 Response *response) {
    FillResponseHeaderBase(slave_address, register_count, response, kFunction);
  }
};

class ReadInputRegistersCommand : public ReadMultipleRegistersCommandBase {
  static const Function kFunction = Function::kReadInputRegisters;

 public:
  static int32_t FillFrame(const uint16_t address, const uint16_t value,
                           Frame *frame) {
    return FillFrameBase(address, value, frame, kFunction);
  }

  static void FillResponseHeader(const uint8_t slave_address,
                                 const std::size_t register_count,
                                 Response *response) {
    FillResponseHeaderBase(slave_address, register_count, response, kFunction);
  }
};

class WriteMultipleHoldingRegistersCommand : public RegisterCommand {
 public:
  static const constexpr auto kFunction =
      Function::kWriteMultipleHoldingRegisters;
  struct CommandPacket {
    static const constexpr std::size_t kRegisterCount =
        DataCommand::CommandPacket::kDataAddressEnd + 1;
    static const constexpr std::size_t kNumberOfDataBytes =
        kRegisterCount + sizeof(uint16_t);
    static const constexpr std::size_t kValueStart = kNumberOfDataBytes + 1;
    static const constexpr std::size_t kHeaderSize = kValueStart;
  };
  struct ResponsePacket {
    static const std::size_t kDataAddress =
        DataCommand::ResponsePacket::kHeaderEnd + 1;
    static const std::size_t kRegisterCount = kDataAddress + sizeof(uint16_t);
    static const constexpr std::size_t kPacketSize =
        kRegisterCount + sizeof(uint16_t);
  };

 public:
  static uint16_t ReadRegisterCount(const ArrayView<uint8_t> &data_array) {
    return Utilities::Make_MSB_uint16_tFromU8Array(ArrayView<const uint8_t>{
        sizeof(uint16_t), &data_array[CommandPacket::kRegisterCount]});
  }
  static std::size_t ReadDataByteCount(const ArrayView<uint8_t> &data_array) {
    return data_array[CommandPacket::kNumberOfDataBytes];
  }
  static uint16_t ReadResponseRegisterCount(
      const ArrayView<uint8_t> &data_array) {
    return Utilities::Make_MSB_uint16_tFromU8Array(ArrayView<const uint8_t>{
        sizeof(uint16_t), &data_array[ResponsePacket::kRegisterCount]});
  }
  static uint16_t ReadResponseAddressStart(
      const ArrayView<uint8_t> &data_array) {
    return Utilities::Make_MSB_uint16_tFromU8Array(ArrayView<const uint8_t>{
        sizeof(uint16_t), &data_array[ResponsePacket::kDataAddress]});
  }

  static void FillResponseHeader(const uint8_t slave_address,
                                 const uint16_t starting_address,
                                 const std::size_t register_count,
                                 Response *response) {
    response->operator[](DataCommand::ResponsePacket::kSlaveAddress) =
        slave_address;
    response->operator[](DataCommand::ResponsePacket::kFunction) =
        static_cast<uint8_t>(kFunction);
    response->operator[](ResponsePacket::kDataAddress) =
        Utilities::GetByte(starting_address, 1);
    response->operator[](ResponsePacket::kDataAddress + 1) =
        Utilities::GetByte(starting_address, 0);
    response->operator[](ResponsePacket::kRegisterCount) =
        Utilities::GetByte(register_count, 1);
    response->operator[](ResponsePacket::kRegisterCount + 1) =
        Utilities::GetByte(register_count, 0);
    response->SetLength(ResponsePacket::kPacketSize);
  }
  static int32_t FillFrame(const uint16_t starting_address,
                           const uint16_t register_count,
                           const ArrayView<const uint16_t> &data,
                           Frame *frame) {
    const uint8_t number_of_data_bytes = data.size() * sizeof(data[0]);
    const std::size_t frame_size =
        CommandPacket::kHeaderSize + number_of_data_bytes;
    assert(frame->data_array.size() >= frame_size);

    frame->function = kFunction;
    frame->data_array[DataCommand::CommandPacket::kDataAddressStart] =
        Utilities::GetByte(starting_address, 1);
    frame->data_array[DataCommand::CommandPacket::kDataAddressStart + 1] =
        Utilities::GetByte(starting_address, 0);
    frame->data_array[CommandPacket::kRegisterCount] =
        Utilities::GetByte(register_count, 1);
    frame->data_array[CommandPacket::kRegisterCount + 1] =
        Utilities::GetByte(register_count, 0);
    frame->data_array[CommandPacket::kNumberOfDataBytes] = number_of_data_bytes;
    frame->data_length = frame_size;

    std::size_t index = CommandPacket::kValueStart;
    for (uint16_t pt : data) {
      frame->data_array[index++] = Utilities::GetByte(pt, 1);
      frame->data_array[index++] = Utilities::GetByte(pt, 0);
    }
    return 0;
  }
};

template <typename T>
class HoldingRegisterController {
  T *register_data_;

  Exception ValidateWriteSingleHoldingRegister(
      const ArrayView<uint8_t> &data_array) const {
    const std::size_t address =
        WriteSingleHoldingRegisterCommand::ReadAddressStart(data_array);
    if (!WriteLocationValid(address, 1)) {
      return Exception::kIllegalDataAddress;
    }
    return Exception::kAck;
  }
  Exception ValidateReadMultipleHoldingRegisters(
      const ArrayView<uint8_t> &data_array) const {
    const std::size_t address =
        ReadMultipleHoldingRegistersCommand::ReadAddressStart(data_array);
    const std::size_t register_count =
        ReadMultipleHoldingRegistersCommand::ReadRegisterCount(data_array);
    if (!ReadLocationValid(address, register_count)) {
      return Exception::kIllegalDataAddress;
    }
    return Exception::kAck;
  }
  Exception ValidateWriteMultipleHoldingRegisters(
      const ArrayView<uint8_t> &data_array) const {
    const std::size_t register_count =
        WriteMultipleHoldingRegistersCommand::ReadRegisterCount(data_array);
    const std::size_t num_data_bytes =
        WriteMultipleHoldingRegistersCommand::ReadDataByteCount(data_array);
    const std::size_t address =
        WriteMultipleHoldingRegistersCommand::ReadAddressStart(data_array);
    if (!WriteLocationValid(address, register_count)) {
      return Exception::kIllegalDataAddress;
    } else if (register_count * sizeof(uint16_t) != num_data_bytes) {
      return Exception::kIllegalDataValue;
    }
    return Exception::kAck;
  }

 public:
  explicit HoldingRegisterController(T *register_data)
      : register_data_{register_data} {}
  T *GetDataStore(void) { return register_data_; }
  bool WriteLocationValid(std::size_t address, std::size_t count) const {
    return register_data_->WriteLocationValid(address, count);
  }
  bool ReadLocationValid(std::size_t address, std::size_t count) const {
    return register_data_->ReadLocationValid(address, count);
  }
  uint16_t ReadRegister(uint16_t address) const {
    return register_data_->GetRegister(address);
  }
  void WriteRegister(uint16_t address, uint16_t value) {
    register_data_->SetRegister(address, value);
  }
  int32_t ReadFrame(const Frame &frame, Response *response) {
    if (frame.function == Function::kWriteSingleHoldingRegister) {
      return RunWriteSingleHoldingRegister(frame, response);
    } else if (frame.function == Function::kReadMultipleHoldingRegisters) {
      return RunReadMultipleHoldingRegisters(frame, response);
    } else if (frame.function == Function::kWriteMultipleHoldingRegisters) {
      return RunWriteMultipleHoldingRegisters(frame, response);
    } else {
      assert(0);
    }
    return -1;
  }
  int32_t RunWriteSingleHoldingRegister(const Frame &frame,
                                        Response *response) {
    const uint16_t address = DataCommand::ReadAddressStart(frame.data_array);
    const uint16_t register_setting =
        WriteSingleHoldingRegisterCommand::ReadSetting(frame.data_array);
    WriteRegister(address, register_setting);
    register_data_->set_register_callback(address, register_setting);
    WriteSingleHoldingRegisterCommand::FillResponseHeader(
        frame.address, address, register_setting, response);
    return 0;
  }

  int32_t RunReadMultipleHoldingRegisters(const Frame &frame,
                                          Response *response) {
    const uint16_t address =
        ReadMultipleHoldingRegistersCommand::ReadAddressStart(frame.data_array);
    const uint16_t register_count =
        ReadMultipleHoldingRegistersCommand::ReadRegisterCount(
            frame.data_array);
    const uint8_t kDataBytes = sizeof(uint16_t) * register_count;
    const std::size_t kPacketSize =
        kDataBytes +
        ReadMultipleHoldingRegistersCommand::ResponsePacket::kHeaderSize +
        Command::kFooterLength;
    response->SetLength(std::min(response->size(), kPacketSize));

    ReadMultipleHoldingRegistersCommand::FillResponseHeader(
        frame.address, register_count, response);

    auto response_data = ArrayView<uint8_t>{
        response->GetLength() -
            ReadMultipleHoldingRegistersCommand::ResponsePacket::kHeaderSize,
        &response->data()[ReadMultipleHoldingRegistersCommand::ResponsePacket::
                              kHeaderSize]};
    ReadRegisters(address, register_count, &response_data);
    return 0;
  }

  int32_t RunWriteMultipleHoldingRegisters(const Frame &frame,
                                           Response *response) {
    const uint16_t address = DataCommand::ReadAddressStart(frame.data_array);
    const uint16_t register_count =
        WriteMultipleHoldingRegistersCommand::ReadRegisterCount(
            frame.data_array);
    // const uint16_t num_data_bytes =
    //   WriteMultipleHoldingRegistersCommand::ReadDataByteCount(frame.data_array);
    WriteMultipleHoldingRegistersCommand::FillResponseHeader(
        frame.address, address, register_count, response);

    const ArrayView<const uint8_t> data_view{
        register_count * sizeof(uint16_t),
        &frame.data_array[WriteMultipleHoldingRegistersCommand::CommandPacket::
                              kValueStart]};
    register_data_->SetRegisters(address, register_count, data_view);
    register_data_->set_registers_callback(address, register_count, data_view);
    return 0;
  }

  void ReadRegisters(const uint16_t starting_address,
                     const uint16_t register_count,
                     ArrayView<uint8_t> *response_data) {
    if (!ReadLocationValid(starting_address, register_count)) {
      assert(0);
      return;
    }
    register_data_->GetRegisters(starting_address, register_count,
                                 response_data);
  }

  Exception ValidateFrame(const Frame &frame) const {
    Exception exception = Exception::kAck;
    if (frame.function == Function::kReadMultipleHoldingRegisters) {
      exception = ValidateReadMultipleHoldingRegisters(frame.data_array);
    } else if (frame.function == Function::kWriteSingleHoldingRegister) {
      exception = ValidateWriteSingleHoldingRegister(frame.data_array);
    } else if (frame.function == Function::kWriteMultipleHoldingRegisters) {
      exception = ValidateWriteMultipleHoldingRegisters(frame.data_array);
    } else {
      assert(0);
    }
    return exception;
  }
};

template <typename T>
class InputRegisterController {
  T *register_data_;

  Exception ValidateReadRegisters(const ArrayView<uint8_t> &data_array) const {
    const std::size_t address =
        ReadInputRegistersCommand::ReadAddressStart(data_array);
    const std::size_t register_count =
        ReadInputRegistersCommand::ReadRegisterCount(data_array);
    if (!ReadLocationValid(address, register_count)) {
      return Exception::kIllegalDataAddress;
    }
    return Exception::kAck;
  }

 public:
  explicit InputRegisterController(T *register_data)
      : register_data_{register_data} {}
  T *GetDataStore(void) { return &register_data_; }
  bool ReadLocationValid(std::size_t address, std::size_t count) const {
    return register_data_->ReadLocationValid(address, count);
  }
  uint16_t ReadRegister(uint16_t address) const {
    return register_data_->GetRegister(address);
  }
  void WriteRegister(uint16_t address, uint16_t value) {
    register_data_->SetRegister(address, value);
  }
  int32_t ReadFrame(const Frame &frame, Response *response) {
    if (frame.function != Function::kReadInputRegisters) {
      return -1;
    }
    return RunReadInputRegisters(frame, response);
  }

  int32_t RunReadInputRegisters(const Frame &frame, Response *response) {
    const uint16_t register_count =
        ReadInputRegistersCommand::ReadRegisterCount(frame.data_array);
    const uint8_t kDataBytes = sizeof(uint16_t) * register_count;
    const std::size_t kPacketSize =
        kDataBytes + ReadInputRegistersCommand::ResponsePacket::kHeaderSize +
        Command::kFooterLength;
    response->SetLength(std::min(response->size(), kPacketSize));
    const uint16_t address =
        ReadInputRegistersCommand::ReadAddressStart(frame.data_array);

    auto response_data = ArrayView<uint8_t>{
        response->GetLength() -
            ReadInputRegistersCommand::ResponsePacket::kHeaderSize,
        &response
             ->data()[ReadInputRegistersCommand::ResponsePacket::kHeaderSize]};
    ReadRegisters(address, register_count, &response_data);
    ReadInputRegistersCommand::FillResponseHeader(frame.address, register_count,
                                                  response);
    return 0;
  }

  void ReadRegisters(const uint16_t starting_address,
                     const uint16_t register_count,
                     ArrayView<uint8_t> *response_data) {
    if (!ReadLocationValid(starting_address, register_count)) {
      assert(0);
      return;
    }
    register_data_->GetRegisters(starting_address, register_count,
                                 response_data);
  }

  Exception ValidateFrame(const Frame &frame) const {
    Exception exception = Exception::kIllegalFunction;
    if (frame.function == Function::kReadInputRegisters) {
      exception = ValidateReadRegisters(frame.data_array);
    } else {
      assert(0);
    }
    return exception;
  }
};

}  //  namespace Modbus
