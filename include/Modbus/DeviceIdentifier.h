/* Copyright (C) 2020 Electrooptical Innovations
 * ----------------------------------------------------------------------
 * Project:      Modbus
 * Title:        DeviceIdentifier.h
 * Description:  Controller for device identifier functions
 *
 * $Date:        13. May 2020
 * $Revision:    V.1.0.1
 * ----------------------------------------------------------------------
 */
#pragma once
#ifndef MODBUS_DEVICEIDENTIFIER_H_
#define MODBUS_DEVICEIDENTIFIER_H_

#include <Modbus/Modbus.h>
#include <ArrayView/ArrayView.h>
#include <cstdint>

namespace Modbus {
inline const constexpr uint8_t kMeiTypeDeviceIdentifier = 0x0e;
enum class ReadDeviceIdCode {
  kBasic = 0x01,
  kRegular = 0x02,
  kExtended = 0x03,
  kSpecificIdentificationId = 0x04,
};
enum class ConformityLevel : uint8_t {
  kBasic = 0x01,
  kRegular = 0x02,
  kExtended = 0x03,
  kBasicIdentification = 0x81,
  kRegularIdentification = 0x82,
  kExtendedIdentification = 0x83,
};
enum class ObjectId : uint8_t {
  kVendorName = 0x00,
  kProductCode = 0x01,
  kMajorMinorRevision = 0x02,
  kVendorUrl = 0x03,
  kProductName = 0x04,
  kModelName = 0x05,
  kUserApplicationName = 0x06
};
struct DeviceIdentifier {
  ArrayView<const char> vendor_name_;
  ArrayView<const char> product_code_;
  ArrayView<const char> major_minor_revision_;
  ArrayView<const char> vendor_url_;
  ArrayView<const char> product_name_;
  ArrayView<const char> model_name_;
  ArrayView<const char> user_application_name_;
  constexpr DeviceIdentifier(const ArrayView<const char>& vendor_name,
                             const ArrayView<const char>& product_code,
                             const ArrayView<const char>& major_minor_revision,
                             const ArrayView<const char>& vendor_url,
                             const ArrayView<const char>& product_name,
                             const ArrayView<const char>& model_name,
                             const ArrayView<const char>& user_application_name)
      : vendor_name_{vendor_name},
        product_code_{product_code},
        major_minor_revision_{major_minor_revision},
        vendor_url_{vendor_url},
        product_name_{product_name},
        model_name_{model_name},
        user_application_name_{user_application_name} {}
};

class DeviceIdentifierController {
  enum class RequestPacket : uint8_t {
    kMeiType, kIdCode, kObjectId
  };
  enum class ResponsePacket : uint8_t {
  kAddress, kFunctionCode,
    kMeiType, kIdCode, kConformityLevel, kMoreFollows,
    kNextObjectId, kNumberOfObjects, kObjectListStart
  };
  const DeviceIdentifier device_data_;

  int RunBasicDeviceIdentification(const Modbus::Frame&, Modbus::Response* response) const {
    const uint8_t number_of_objects = 3;
    //  assert(frame.function == Modbus::Function::kReadDeviceIdentification);
    response->operator[](static_cast<uint8_t>(ResponsePacket::kMeiType)) =
      kMeiTypeDeviceIdentifier;
    response->operator[](static_cast<uint8_t>(ResponsePacket::kIdCode)) =
      static_cast<uint8_t>(ReadDeviceIdCode::kBasic);
    response->operator[](static_cast<uint8_t>(ResponsePacket::kConformityLevel)) =
      static_cast<uint8_t>(ConformityLevel::kBasic);
    response->operator[](static_cast<uint8_t>(ResponsePacket::kMoreFollows)) = 0x00;
    response->operator[](static_cast<uint8_t>(ResponsePacket::kNextObjectId)) =
      static_cast<uint8_t>(ObjectId::kVendorName);
    response->operator[](static_cast<uint8_t>(ResponsePacket::kNumberOfObjects)) = number_of_objects;

    std::size_t index = static_cast<uint8_t>(ResponsePacket::kObjectListStart);

    response->operator[](index++) = static_cast<uint8_t>(ObjectId::kVendorName);

    response->operator[](index++) = device_data_.vendor_name_.size();
    for (auto pt : device_data_.vendor_name_) {
      response->operator[](index++) = static_cast<uint8_t>(pt);
    }

    response->operator[](index++) = static_cast<uint8_t>(ObjectId::kProductCode);
    response->operator[](index++) = device_data_.product_code_.size();
    for (auto pt : device_data_.product_code_) {
      response->operator[](index++) = static_cast<uint8_t>(pt);
    }

    response->operator[](index++) = static_cast<uint8_t>(ObjectId::kMajorMinorRevision);
    response->operator[](index++) = device_data_.major_minor_revision_.size();
    for (auto pt : device_data_.major_minor_revision_) {
      response->operator[](index++) = static_cast<uint8_t>(pt);
    }
    response->SetLength(index);
    return 0;
  }

 public:
  static void FrameRequest(ArrayView<uint8_t>* view, ReadDeviceIdCode type_code = ReadDeviceIdCode::kBasic, uint8_t object_id = 0x00) {
    view->operator[](static_cast<uint8_t>(RequestPacket::kMeiType)) = kMeiTypeDeviceIdentifier;
    view->operator[](static_cast<uint8_t>(RequestPacket::kIdCode)) = static_cast<uint8_t>(type_code);
    view->operator[](static_cast<uint8_t>(RequestPacket::kObjectId)) = object_id;
  }
  static Modbus::Exception ValidateFrame(const Frame& frame) {
    Modbus::Exception exception = Modbus::Exception::kAck;
    if (frame.data_array[static_cast<uint8_t>(RequestPacket::kMeiType)]
        != kMeiTypeDeviceIdentifier) {
        exception = Modbus::Exception::kIllegalDataAddress;
    } else if (frame.data_array[static_cast<uint8_t>(RequestPacket::kIdCode)] !=
        static_cast<uint8_t>(ReadDeviceIdCode::kBasic)) {
      exception = Modbus::Exception::kIllegalDataAddress;
    }
    return exception;
  }
  int ReadFrame(const Modbus::Frame& frame, Modbus::Response* response) {
    if (frame.function == Modbus::Function::kReadDeviceIdentification) {
      return RunBasicDeviceIdentification(frame, response);
    }
    assert(0);
    return -1;
  }
  constexpr explicit DeviceIdentifierController(const DeviceIdentifier& device_data) :
    device_data_{device_data} {}
};
}  // namespace Modbus

#endif  //  MODBUS_DEVICEIDENTIFIER_H_
