#pragma once
#include <cstddef>
#include <cstdint>
#include <cassert>
#include <cstring>
#include <climits>
#include <algorithm>
#include <Utilities/Serializer.h>

#define BYTE_SIZE 8
#define REGISTER_SIZE 2

struct u8_Buffer {
  uint8_t* buffer;
  size_t size;
};

struct U8Field {
  size_t address;
  size_t length;
  u8_Buffer* buffer;
};

