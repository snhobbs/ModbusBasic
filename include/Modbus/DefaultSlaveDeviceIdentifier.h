/*
 * Copyright 2020 ElectroOptical Innovations, LLC
 * */
#pragma once
#include <Modbus/DeviceIdentifier.h>
#include <ArrayView/ArrayView.h>
#include <array>
#include <cstdint>

class SlaveDeviceIdentifier {
  static const constexpr char vendor_name_[] = "Vendor Name";
  static const constexpr char product_code_[] = "Product Code";
  static const constexpr char major_minor_revision_[] = "MajorMinorRevision";
  static const constexpr char vendor_url_[] = "Vendor URL";
  static const constexpr char product_name_[] = "Product Name";
  static const constexpr char model_name_[] = "Model Name";
  static const constexpr char user_application_name_[] = "User Application Name";

 public:
  static Modbus::DeviceIdentifier GetDeviceIdentifier(void) {
    return Modbus::DeviceIdentifier {
      {sizeof(vendor_name_), vendor_name_},
      {sizeof(product_code_), product_code_},
      {sizeof(major_minor_revision_), major_minor_revision_},
      {sizeof(vendor_url_), vendor_url_},
      {sizeof(product_name_), product_name_},
      {sizeof(model_name_), model_name_},
      {sizeof(user_application_name_), user_application_name_},
    };
  }
};
