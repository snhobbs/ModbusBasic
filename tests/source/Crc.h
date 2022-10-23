/*
 * Copyright 2020 ElectroOptical Innovations, LLC
 * */
#pragma once
#ifndef CRC_H_
#define CRC_H_

//#include <ArrayView/ArrayView.h>
#include <Utilities/Crc.h>

#include <cassert>

//  Duck type array/arrayview interface
template <typename T>
inline uint16_t crc16(const T &array, std::size_t length) {
  assert(length <= array.size());
  return Utilities::crc16(array.data(), length);
}

#endif  //  CRC_H_
