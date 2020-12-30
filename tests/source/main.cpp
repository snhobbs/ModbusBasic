/*
 * Copyright 2020 Electrooptical Innovations
 * main.cpp

 *
 *      Author: simon
 */

#include <gtest/gtest.h>
#include <stdint.h>
#include <array>
#include <iostream>
#include <vector>
TEST(Sanity, Check) { ASSERT_EQ(1, 1); }

int main(int argc, char* argv[]) {
  testing::InitGoogleTest(&argc, argv);
  for (auto i = 0; i < argc; i++) {
    std::cout << argv[i] << '\n';
  }
  std::cout << "Compiled: " << __TIME__ << '\t' << __DATE__ << std::endl;
  return RUN_ALL_TESTS();
}
