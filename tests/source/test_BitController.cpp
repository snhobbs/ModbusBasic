#include "Modbus/BitController.h"
#include <gtest/gtest.h>
#include <array>

using namespace Modbus;
TEST(fields_are_sorted, blocks_in_order_passes) {
  std::array<uint8_t, 64> block_buffer_0;
  std::array<uint8_t, 64> block_buffer_1;
  u8_Buffer buffer_0{block_buffer_0.data(), block_buffer_0.size()};
  u8_Buffer buffer_1{block_buffer_1.data(), block_buffer_1.size()};

  BitField bf_0{0, block_buffer_0.size()*BYTE_SIZE, &buffer_0};
  BitField bf_1{bf_0.length, block_buffer_1.size()*BYTE_SIZE, &buffer_1};

  std::array<BitField*, 2> bf {&bf_0, &bf_1};
  EXPECT_TRUE(fields_are_sorted(bf.data(), bf.size()));
}

TEST(fields_are_sorted, blocks_out_of_order_fails) {
  std::array<uint8_t, 64> block_buffer_0;
  std::array<uint8_t, 64> block_buffer_1;
  u8_Buffer buffer_0{block_buffer_0.data(), block_buffer_0.size()};
  u8_Buffer buffer_1{block_buffer_1.data(), block_buffer_1.size()};

  BitField bf_0{0, block_buffer_0.size()*BYTE_SIZE, &buffer_0};
  BitField bf_1{bf_0.length, block_buffer_1.size()*BYTE_SIZE, &buffer_1};

  std::array<BitField*, 2> bf {&bf_1, &bf_0}; // Swap positions of bf_0 & 1
  EXPECT_FALSE(fields_are_sorted(bf.data(), bf.size()));
}

TEST(BitController, instantiate_null) {
  BitController bc{nullptr, 0};
}

class Test_BitController: public testing::Test {
  public:
    std::array<uint8_t, 64> block_buffer_0;
    std::array<uint8_t, 64> block_buffer_1;
    u8_Buffer buffer_0{block_buffer_0.data(), block_buffer_0.size()};
    u8_Buffer buffer_1{block_buffer_1.data(), block_buffer_1.size()};

    BitField bf_0{0, block_buffer_0.size()*BYTE_SIZE, &buffer_0};
    BitField bf_1{bf_0.length, block_buffer_1.size()*BYTE_SIZE, &buffer_1};
    std::array<BitField*, 2> bf{&bf_0, &bf_1};
    BitController bc{bf.data(), bf.size()};

    void setUp() {}
    void tearDown() {}
};

TEST_F(Test_BitController, interblock_access_valid) {
  EXPECT_TRUE(bc.access_valid(bf_0.address + 1, bf_0.length-1));
}

TEST_F(Test_BitController, overrunblock_access_invalid) {
  EXPECT_FALSE(bc.access_valid(bf_0.address + 1, bf_0.length));
}

TEST_F(Test_BitController, nomatching_block_access_invalid) {
  EXPECT_FALSE(bc.access_valid(bf_1.address + bf_1.length, 1));
}

TEST_F(Test_BitController, valid_write_byte_count) {
  std::array<uint8_t, 2> data {0xff, 0xaa};
  const u8_Buffer buff{data.data(), data.size()};
  const size_t bits_written = bc.write(bf_0.address, buff.size*BYTE_SIZE, &buff);
  EXPECT_EQ(bits_written, data.size()*BYTE_SIZE);
}

TEST_F(Test_BitController, valid_write_byte_data_as_expected) {
  std::array<uint8_t, 9> data {1, 2, 3, 4, 5, 6, 7, 8, 9};
  const u8_Buffer buff{data.data(), data.size()};
  const size_t bits_written = bc.write(bf_0.address, buff.size*BYTE_SIZE, &buff);
  for (size_t i=0; i<data.size(); i++) {
    EXPECT_EQ(bf_0.buffer->buffer[i], data[i]);
  }
}

TEST_F(Test_BitController, valid_read_byte_count) {
  std::array<uint8_t, 9> data {1, 2, 3, 4, 5, 6, 7, 8, 9};
  u8_Buffer buff{data.data(), data.size()};
  const size_t bits_read = bc.read(bf_0.address, buff.size*BYTE_SIZE, &buff);
  EXPECT_EQ(bits_read, data.size()*BYTE_SIZE);
}

TEST_F(Test_BitController, valid_read_data_as_expected) {
  std::array<uint8_t, 9> data {1, 2, 3, 4, 5, 6, 7, 8, 9};  //  Set to non zero, check they get cleared
  u8_Buffer buff{data.data(), data.size()};
  const size_t bits_read = bc.read(bf_0.address, buff.size*BYTE_SIZE, &buff);
  for (size_t i=0; i<data.size(); i++) {
    EXPECT_EQ(bf_0.buffer->buffer[i], data[i]);
  }
}
