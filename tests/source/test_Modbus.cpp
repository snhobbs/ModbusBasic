#include <gtest/gtest.h>

#include "Modbus/Modbus.h"

using namespace Modbus;

TEST(Modbus, FunctionCodeIsValid_InValidFunctionsReturnsFalse) {
  const uint8_t code = 255;
  EXPECT_FALSE(FunctionCodeIsValid(code));
}

TEST(Modbus, GetErrorFunction_kNoneReturnskNone) {
  const Modbus::Function write_function = Modbus::Function::kNone;
  const Modbus::Function function = GetErrorFunction(write_function);
  EXPECT_EQ(Modbus::Function::kNone, function);
}

TEST(Modbus, GetFunction_InvalidReturnsError) {
  const Modbus::Function function = GetFunction(0xfa);
  EXPECT_EQ(Modbus::Function::kNone, function);
}

TEST(
    Modbus,
    GetAddressSpaceFromFunction_WriteSingleHoldingRegisterReturnsHoldingRegister) {
  const auto address = GetAddressSpaceFromFunction(
      Modbus::Function::kWriteSingleHoldingRegister);
  EXPECT_EQ(address, Modbus::AddressSpace::kHoldingRegister);
}

TEST(Modbus,
     GetAddressSpaceFromFunction_SupportedFunctionsDoesNotReturnUnmapped) {
  for (auto& pt : GetSupportedFunctions()) {
    const Modbus::AddressSpace address = GetAddressSpaceFromFunction(pt);
    EXPECT_NE(address, AddressSpace::kUnmapped);
  }
}
