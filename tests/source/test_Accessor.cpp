#include "Modbus/Modbus.h"
#include "Modbus/Accessor.h"
#include "Modbus/BitController.h"
#include "Modbus/RegisterController.h"
#include <gtest/gtest.h>
#include <array>

using namespace Modbus;
TEST(Accessor, instantiate) {
  BitController coil{nullptr, 0};
  BitController dinput{nullptr, 0};
  RegisterController holding{nullptr, 0};
  RegisterController input_register{nullptr, 0};
}

class Accessor_Test : public testing::Test {
 public:
  std::array<uint8_t, 32> coil_array;
  u8_Buffer coil_buffer {coil_array.data(), coil_array.size()};
  BitField coil_bf{0, coil_buffer.size*BYTE_SIZE, &coil_buffer};
  std::array<BitField*, 1> coil_fields {&coil_bf};
  BitController coil{coil_fields.data(), coil_fields.size()};

  std::array<uint8_t, 32> discrete_input_array;
  u8_Buffer discrete_input_buffer {discrete_input_array.data(), discrete_input_array.size()};
  BitField discrete_input_bf{0, discrete_input_buffer.size*BYTE_SIZE, &discrete_input_buffer};
  std::array<BitField*, 1> discrete_input_fields {&discrete_input_bf};
  BitController discrete_input{discrete_input_fields.data(), discrete_input_fields.size()};

  std::array<uint8_t, 32> holding_register_array;
  u8_Buffer holding_register_buffer {holding_register_array.data(), holding_register_array.size()};
  RegisterField holding_register_bf{0, holding_register_buffer.size*BYTE_SIZE, &holding_register_buffer};
  std::array<RegisterField*, 1> holding_register_fields {&holding_register_bf};
  RegisterController holding_register{holding_register_fields.data(), holding_register_fields.size()};

  std::array<uint8_t, 32> input_register_array;
  u8_Buffer input_register_buffer {input_register_array.data(), input_register_array.size()};
  RegisterField input_register_bf{0, input_register_buffer.size*BYTE_SIZE, &input_register_buffer};
  std::array<RegisterField*, 1> input_register_fields {&input_register_bf};
  RegisterController input_register{input_register_fields.data(), input_register_fields.size()};

  static const constexpr std::array<AddressSpace, 4> kAddresses {
      AddressSpace::kCoil,
      AddressSpace::kDiscreteInput,
      AddressSpace::kHoldingRegister,
      AddressSpace::kInputRegister};

  Accessor accessor{&coil, &discrete_input, &holding_register, &input_register};
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
  u8_Buffer buffer {data_array.data(), data_array.size()};
  for (auto field : kAddresses) {
    const size_t fields = accessor.read(0, 1, field, &buffer);
    EXPECT_EQ(fields, 1);
  }
}

TEST_F(Accessor_Test, valid_write_returns_number_of_fields) {
  std::array<uint8_t, 128> data_array{};
  const u8_Buffer buffer {data_array.data(), data_array.size()};
  for (auto field : kAddresses) {
    const size_t fields = accessor.write(0, 1, field, &buffer);
    EXPECT_EQ(fields, 1);
  }
}
