#include <Modbus/RegisterController.h>
#include <gtest/gtest.h>

#include <array>
#include <cstdio>

using namespace Modbus;

TEST(RegisterControllerfields_are_sorted, blocks_in_order_passes) {
  std::array<uint8_t, 64> block_buffer_0;
  std::array<uint8_t, 64> block_buffer_1;
  u8_Buffer buffer_0{block_buffer_0.data(), block_buffer_0.size()};
  u8_Buffer buffer_1{block_buffer_1.data(), block_buffer_1.size()};

  U8Field rf_0{0, block_buffer_0.size() * BYTE_SIZE, &buffer_0};
  U8Field rf_1{rf_0.length, block_buffer_1.size() * BYTE_SIZE, &buffer_1};

  std::array<U8Field*, 2> rf{&rf_0, &rf_1};
  EXPECT_TRUE(fields_are_sorted(rf.data(), rf.size()));
}

TEST(RegisterControllerfields_are_sorted, blocks_out_of_order_fails) {
  std::array<uint8_t, 64> block_buffer_0;
  std::array<uint8_t, 64> block_buffer_1;
  u8_Buffer buffer_0{block_buffer_0.data(), block_buffer_0.size()};
  u8_Buffer buffer_1{block_buffer_1.data(), block_buffer_1.size()};

  U8Field rf_0{0, block_buffer_0.size() * BYTE_SIZE, &buffer_0};
  U8Field rf_1{rf_0.length, block_buffer_1.size() * BYTE_SIZE, &buffer_1};

  std::array<U8Field*, 2> rf{&rf_1, &rf_0};  // Swap positions of rf_0 & 1
  EXPECT_FALSE(fields_are_sorted(rf.data(), rf.size()));
}

TEST(RegisterController, instantiate_null) {
  RegisterController rc{nullptr, 0};
}

class Test_RegisterController : public testing::Test {
 public:
  std::array<uint8_t, 64> block_buffer_0{};
  std::array<uint8_t, 64> block_buffer_1{};
  u8_Buffer buffer_0{block_buffer_0.data(), block_buffer_0.size()};
  u8_Buffer buffer_1{block_buffer_1.data(), block_buffer_1.size()};

  U8Field rf_0{0, block_buffer_0.size() * BYTE_SIZE, &buffer_0};
  U8Field rf_1{rf_0.length, block_buffer_1.size() * BYTE_SIZE, &buffer_1};
  std::array<U8Field*, 2> rf{&rf_0, &rf_1};
  RegisterController rc{rf.data(), rf.size()};

  void setUp() {}
  void tearDown() {}
};

TEST_F(Test_RegisterController, interblock_access_valid) {
  EXPECT_TRUE(rc.access_valid(rf_0.address + 1, rf_0.length - 1));
}

TEST_F(Test_RegisterController, overrunblock_access_invalid) {
  EXPECT_FALSE(rc.access_valid(rf_0.address + 1, rf_0.length));
}

TEST_F(Test_RegisterController, nomatching_block_access_invalid) {
  EXPECT_FALSE(rc.access_valid(rf_1.address + rf_1.length, 1));
}

TEST_F(Test_RegisterController, valid_write_byte_count) {
  std::array<uint8_t, 4> data{0xff, 0xfa, 0xad, 0xdd};
  const u8_Buffer buff{data.data(), data.size()};
  const size_t write_size = 1;
  const size_t registers_written = rc.write(rf_0.address, write_size, &buff);
  EXPECT_EQ(registers_written, write_size);
}

TEST_F(Test_RegisterController, invalid_write_byte_count_is_zero) {
  std::array<uint8_t, 64> data{0xff, 0xfa, 0xad, 0xdd};
  const u8_Buffer buff{data.data(), data.size()};
  const size_t registers_written =
      rc.write(rf_0.address, rf_0.length + 1, &buff);
  EXPECT_EQ(registers_written, 0);
}

TEST_F(Test_RegisterController, valid_write_byte_data_as_expected) {
  std::array<uint8_t, 4> data{0xff, 0xfa, 0xad, 0xdd};
  const u8_Buffer buff{data.data(), data.size()};
  const size_t write_size = data.size() / REGISTER_SIZE;
  const size_t registers_written = rc.write(rf_0.address, write_size, &buff);
  assert(registers_written == write_size);

  for (size_t i = 0; i < data.size(); i++) {
    EXPECT_EQ(rf_0.buffer->buffer[i], data[i]);
  }
}

TEST_F(Test_RegisterController, valid_read_byte_count) {
  std::array<uint8_t, 4> data{0xff, 0xfa, 0xad, 0xdd};
  u8_Buffer buff{data.data(), data.size()};
  const size_t registers_read =
      rc.read(rf_0.address, buff.size / REGISTER_SIZE, &buff);
  EXPECT_EQ(registers_read, data.size() / REGISTER_SIZE);
}

TEST_F(Test_RegisterController, valid_read_data_as_expected) {
  std::array<uint8_t, 4> data{0xff, 0xfa, 0xad, 0xdd};
  u8_Buffer buff{data.data(), data.size()};
  const size_t registers_read =
      rc.read(rf_0.address, buff.size / REGISTER_SIZE, &buff);
  for (size_t i = 0; i < data.size(); i++) {
    EXPECT_EQ(rf_0.buffer->buffer[i], data[i]);
  }
}
