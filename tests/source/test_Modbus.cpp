#include "Modbus/Modbus.h"
#include <gtest/gtest.h>

using namespace Modbus;
TEST(Modbus, FunctionCodeIsValid_ValidFunctionsReturnsTrue) {
  const uint8_t code = static_cast<uint8_t>(Modbus::Function::kReadCoils);
  EXPECT_TRUE(FunctionCodeIsValid(code));
}

TEST(Modbus, FunctionCodeIsValid_InValidFunctionsReturnsFalse) {
  const uint8_t code = 255;
  EXPECT_FALSE(FunctionCodeIsValid(code));
}

TEST(Modbus, GetErrorFunction_ValidReturnsSameWithAddValue) {
  const Modbus::Function write_function = Modbus::Function::kReadCoils;
  const Modbus::Function function = GetErrorFunction(write_function);
  EXPECT_EQ(static_cast<size_t>(write_function) + Modbus::kStatusResponseAddValue, static_cast<size_t>(function));
}

TEST(Modbus, GetErrorFunction_ValidReturnsCorrespondingError) {
  const Modbus::Function function = GetErrorFunction(Modbus::Function::kReadCoils);
  EXPECT_EQ(function, Modbus::Function::kErrorReadCoils);
}

TEST(Modbus, GetErrorFunction_ErrorFunction_ReturnsSame) {
  const Modbus::Function write_function = Modbus::Function::kErrorReadCoils;
  const Modbus::Function function = GetErrorFunction(write_function);
  EXPECT_EQ(write_function, function);
}

TEST(Modbus, GetErrorFunction_kNoneReturnskNone) {
  const Modbus::Function write_function = Modbus::Function::kNone;
  const Modbus::Function function = GetErrorFunction(write_function);
  EXPECT_EQ(Modbus::Function::kNone, function);
}

TEST(Modbus, GetFunction_ValidReturnsSame) {
  const Modbus::Function write_function = Modbus::Function::kReadCoils;
  const Modbus::Function function = GetFunction(static_cast<uint8_t>(write_function));
  EXPECT_EQ(write_function, function);
}
TEST(Modbus, GetFunction_InvalidReturnsError) {
  const Modbus::Function function = GetFunction(0xfa);
  EXPECT_EQ(Modbus::Function::kNone, function);
}

TEST(Modbus, GetAddressSpaceFromFunction_ReadCoilsReturnCoils) {
  const auto address = GetAddressSpaceFromFunction(Modbus::Function::kReadCoils);
  EXPECT_EQ(address, Modbus::AddressSpace::kCoil);
}
TEST(Modbus, GetAddressSpaceFromFunction_WriteSingleHoldingRegisterReturnsHoldingRegister) {
  const auto address = GetAddressSpaceFromFunction(Modbus::Function::kWriteSingleHoldingRegister);
  EXPECT_EQ(address, Modbus::AddressSpace::kHoldingRegister);
}

TEST(Modbus, GetAddressSpaceFromFunction_SupportedFunctionsDoesNotReturnUnmapped) {
  for (auto& pt : GetSupportedFunctions()) {
    const Modbus::AddressSpace address = GetAddressSpaceFromFunction(pt);
    EXPECT_NE(address, AddressSpace::kUnmapped);
  }
}
