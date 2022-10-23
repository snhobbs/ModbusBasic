/*
 * Copyright 2020 Electrooptical Innovations
 * test_buffer.cpp
 *
 *  Created on: Jun 27, 2018
 *      Author: simon
 */
#include <gtest/gtest.h>

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "RingBuffer/RingBuffer.h"

class RingBuffSetup : public ::testing::Test {
 public:
  static const std::size_t kBuffSize = 4096;
  RingBuffSetup(void) {}
  ~RingBuffSetup(void) {}
  void SetUp(void) {
    // code here will execute just before the test ensues
  }

  void TearDown(void) {
    // code here will be called just after the test completes
    // ok to through exceptions from here if need be
  }
};

TEST_F(RingBuffSetup, Count) {
  RingBuffer<char, kBuffSize> buff;
  for (std::size_t i = 0; i < kBuffSize * 5;) {
    i++;
    buff.insert(static_cast<char>((i + 1) & 0xff));
    if (i > buff.size()) {
      EXPECT_EQ(buff.size(), buff.GetCount());
      EXPECT_TRUE(buff.isFull());
    } else {
      EXPECT_EQ(i, buff.GetCount());
    }
  }
}

TEST_F(RingBuffSetup, Peek) {
  RingBuffer<char, kBuffSize> buff;
  std::string txt = "Hello World, This is the test data";
  for (const char &b : txt) {
    buff.insert(b);
  }
  for (std::size_t i = 0; i < buff.GetCount(); i++) {
    char ch = '\0';
    buff.peek(&ch, i);
    EXPECT_EQ(ch, txt[txt.size() - i]);
  }
}

template <typename T>
uint32_t MaskIndex(const T &buffer, uint32_t index) {
  return (buffer.size() - 1) & index;
}

TEST_F(RingBuffSetup, PeekLong) {
  RingBuffer<uint32_t, kBuffSize> buff;
  std::vector<uint32_t> data{0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10,
                             11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
  std::vector<uint32_t> long_data;
  while (long_data.size() <= kBuffSize * 2) {
    long_data.insert(long_data.end(), data.begin(), data.end());
  }
  // std::size_t count = 0;
  for (auto b : long_data) {
    // count++;
    buff.insert(b);
  }

  EXPECT_EQ(true, buff.isFull());
  EXPECT_EQ(false, buff.isEmpty());
  EXPECT_TRUE(buff.GetCount() == buff.GetSize());

  for (std::size_t i = 0; i < buff.size(); i++) {
    uint32_t dp = 0;
    buff.peek(&dp, buff.size() - i);
    EXPECT_EQ(dp, long_data[i]);
  }
}

TEST_F(RingBuffSetup, InsertMulti) {
  RingBuffer<char, kBuffSize> buff;
  std::string txt = "Hello World, This is the test data";
  std::string long_data;

  while (long_data.size() < kBuffSize) {
    long_data.append(txt);
  }
  for (auto b : long_data) {
    // count++;
    buff.insert(b);
  }

  EXPECT_EQ(true, buff.isFull());
  EXPECT_EQ(false, buff.isEmpty());
  EXPECT_TRUE(buff.GetCount() == buff.GetSize());

  for (std::size_t i = 0; i < buff.size(); i++) {
    char dp = '\0';
    buff.peek(&dp, buff.size() - i);
    EXPECT_EQ(dp, long_data[i]);
  }

  buff.insert(const_cast<char *>(long_data.data()), kBuffSize);  // Overruns!
}

TEST_F(RingBuffSetup, InsertOverRun) {
  RingBuffer<char, kBuffSize> buff;
  std::string txt = "Hello World, This is the test data";
  std::string longTxt;
  std::vector<char> outTxt;
  outTxt.reserve(kBuffSize);

  while (longTxt.size() <= kBuffSize) {  // longtext is longer than buffer
    longTxt.append(txt);
  }

  EXPECT_TRUE((longTxt.size()) > (kBuffSize));

  for (auto &&ch : longTxt) {
    buff.insertOverwrite(std::move(ch));
  }
  buff.pop(outTxt.data(), kBuffSize);

  for (std::size_t i = 0; i < outTxt.size(); i++) {
    EXPECT_EQ(outTxt[outTxt.size() - 1 - i], longTxt[longTxt.size() - 1 - i]);
  }
}

TEST_F(RingBuffSetup, Pop) {
  RingBuffer<uint32_t, kBuffSize> buff;
  std::vector<uint32_t> long_data;

  uint32_t i = 0;
  while (long_data.size() < kBuffSize) {
    long_data.push_back(i++);
  }

  for (auto b : long_data) {
    EXPECT_EQ(false, buff.isFull());
    buff.insert(b);
  }

  EXPECT_EQ(true, buff.isFull());
  EXPECT_EQ(false, buff.isEmpty());
  EXPECT_TRUE(buff.GetCount() == buff.GetSize());

  std::reverse(long_data.begin(), long_data.end());
  while (!buff.isEmpty()) {
    uint32_t dp = 0;
    buff.pop(&dp);
    EXPECT_EQ(dp, long_data.back());
    long_data.pop_back();
  }
}

TEST_F(RingBuffSetup, PopMulti) {
  RingBuffer<char, kBuffSize> buff;
  std::string txt = "Hello World, This is the test data";
  std::string longTxt;
  std::vector<char> outTxt;
  outTxt.reserve(kBuffSize);

  while (longTxt.size() <= kBuffSize) {
    longTxt.append(txt);
  }
  buff.insert(longTxt.c_str(), kBuffSize);
  buff.pop(outTxt.data(), kBuffSize);
  for (std::size_t i = 0; i < buff.GetCount(); i++) {
    EXPECT_EQ(outTxt[i], longTxt[i]);
  }
}
