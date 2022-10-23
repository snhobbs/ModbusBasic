/*
 * Copyright 2020 Electrooptical Innovations
 * test_ringbuffer.cpp
 *
 *  Created on: Jun 26, 2018
 *      Author: simon
 */
#if 1
#include <RingBuffer/RingBuffer.h>
#include <gtest/gtest.h>

#include <array>
#include <iostream>
#include <string>

struct RingBufferSetup : public ::testing::Test {
  const std::string chars =
      "abcbefghijklmnopqrstuvwx1234567890abcbefghijklmnopqrstuvwx1234567890";
  static const constexpr std::size_t Size{(1 << 10)};
  static const constexpr std::size_t ksize = 1 << 10;
  std::array<char, ksize> dout{};

  void fillStr(std::string* str, std::size_t minlen) {
    while (str->size() < minlen) {
      str->append(chars);
    }
  }

  RingBuffer<char, ksize> rb;
};
TEST_F(RingBufferSetup, RB_PopSingle) {
  std::string longstr;
  fillStr(&longstr, 3 * rb.size());

  char out = 0;
  // Test popping over more than the range of the buffer
  for (auto d : chars) {
    rb.insert(d);
    rb.pop(&out);
    EXPECT_EQ(static_cast<std::size_t>(d), static_cast<std::size_t>(out));
  }
}

TEST_F(RingBufferSetup, RB_InsertSingle) {
  // Insert too much  and ensure the output is truncated as expected
  std::string longstr;
  fillStr(&longstr, 3 * rb.size());

  assert(longstr.size() > 0);
  for (auto d : longstr) {
    rb.insert(d);
  }

  std::size_t cnt = 0;
  char out = 0;
  while (rb.pop(&out)) {
    dout[cnt] = out;
    cnt++;
  }
  cnt = 0;
  for (auto d : dout) {
    EXPECT_EQ(static_cast<std::size_t>(longstr[cnt]),
              static_cast<std::size_t>(d));
    cnt++;
  }
}

#if 1
TEST_F(RingBufferSetup, RB_Peek) {
  std::string longstr;
  fillStr(&longstr, 3 * rb.size());

  for (auto d : longstr) {
    rb.insert(d);
  }

  for (std::size_t i = 0; i < rb.GetCount(); i++) {
    char out = 0;
    rb.peek(&out, rb.GetCount() - i);
    EXPECT_EQ(out, longstr[i]);
  }
}
#endif
TEST_F(RingBufferSetup, RB_InsertMulti) {
  std::string longstr;
  fillStr(&longstr, 3 * rb.size());

  auto bytes = rb.insert(longstr.data(), longstr.size());

  EXPECT_EQ(bytes, rb.GetCount());  // The amount filled is the amount reported
  EXPECT_EQ(bytes, rb.size());      // The entire data buffer is filled

  for (std::size_t i = 0; i < rb.GetCount(); i++) {
    char out = 0;
    rb.peek(&out, rb.GetCount() - i);
    EXPECT_EQ(out, longstr[i]);  // Each of the values matches
  }
}

#if 1
TEST_F(RingBufferSetup, RB_PopMulti) {
  std::string longstr;
  fillStr(&longstr, 3 * rb.size());

  auto bytes = rb.insert(longstr.data(), longstr.size());

  rb.pop(dout.data(), bytes);

  for (std::size_t i = 0; i < dout.size(); i++) {
    EXPECT_EQ(dout[i], longstr[i]);
  }
}
#endif
#if 1
TEST_F(RingBufferSetup, RB_InsertOverRun) {
  std::string longstr;
  fillStr(&longstr, 3 * rb.size());

  std::size_t bytes = 0;
  for (auto ch : longstr) {
    bytes += rb.insertOverwrite(ch);
  }

  EXPECT_EQ(bytes, longstr.size());

  rb.pop(dout.data(), bytes);

  for (std::size_t i = 0; i < dout.size(); i++) {
    EXPECT_EQ(dout[dout.size() - 1 - i], longstr[longstr.size() - 1 - i]);
  }
  EXPECT_TRUE(dout.size() < longstr.size());
}
#endif

#endif
