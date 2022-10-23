#include <gtest/gtest.h>

#include <array>

#include "Modbus/Accessor.h"
#include "Modbus/Modbus.h"
#include "Modbus/RegisterController.h"

using namespace Modbus;
TEST(Accessor, instantiate) {
  RegisterController holding{nullptr, 0};
  RegisterController input_register{nullptr, 0};
}

class Accessor_Test : public testing::Test {
 public:
  std::array<uint8_t, 32> holding_register_array;
  u8_Buffer holding_register_buffer{holding_register_array.data(),
                                    holding_register_array.size()};
  U8Field holding_register_bf{0, holding_register_buffer.size* BYTE_SIZE,
                              &holding_register_buffer};
  std::array<U8Field*, 1> holding_register_fields{&holding_register_bf};
  RegisterController holding_register{holding_register_fields.data(),
                                      holding_register_fields.size()};

  std::array<uint8_t, 32> input_register_array;
  u8_Buffer input_register_buffer{input_register_array.data(),
                                  input_register_array.size()};
  U8Field input_register_bf{0, input_register_buffer.size* BYTE_SIZE,
                            &input_register_buffer};
  std::array<U8Field*, 1> input_register_fields{&input_register_bf};
  RegisterController input_register{input_register_fields.data(),
                                    input_register_fields.size()};

  static const constexpr std::array<AddressSpace, 2> kAddresses{
      AddressSpace::kHoldingRegister, AddressSpace::kInputRegister};

  Accessor accessor{&holding_register, &input_register};
};

TEST_F(Accessor_Test, valid_access_returns_true) {
  std::array<uint8_t, 128> data_array{};
  for (auto field : kAddresses) {
    const size_t fields = accessor.access_valid(0, 1, field);
    EXPECT_EQ(fields, 1);
  }
}

TEST_F(Accessor_Test, valid_read_returns_number_of_fields) {
  std::array<uint8_t, 128> data_array{};
  u8_Buffer buffer{data_array.data(), data_array.size()};
  for (auto field : kAddresses) {
    const size_t fields = accessor.read(0, 1, field, &buffer);
    EXPECT_EQ(fields, 1);
  }
}

TEST_F(Accessor_Test, valid_write_returns_number_of_fields) {
  std::array<uint8_t, 128> data_array{};
  const u8_Buffer buffer{data_array.data(), data_array.size()};
  for (auto field : kAddresses) {
    const size_t fields = accessor.write(0, 1, field, &buffer);
    EXPECT_EQ(fields, 1);
  }
}
