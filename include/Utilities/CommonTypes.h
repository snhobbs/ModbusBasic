/*
 * Copyright 2020 ElectroOptical Innovations, LLC
 *
 * */
#pragma once

#ifndef UTILITIES_COMMONTYPES_H_
#define UTILITIES_COMMONTYPES_H_
#include <cassert>
#include <cstdint>
#include <array>
namespace Utilities {

template<typename T = std::size_t>
class Range {
  const T low_;
  const T high_;
 public:
  constexpr T GetLow(void) const {
    return low_;
  }
  constexpr T GetHigh(void) const {
    return high_;
  }
  constexpr Range(T low, T high) : low_{low}, high_{high} {
    assert(low < high);
  }
};

template<typename T>
class StateController {
  T state_ = T::Initial;
  std::size_t state_time_start_ = 0;

 public:
  std::size_t get_state_time_start(void) const {
    return state_time_start_;
  }
  T get(void) const { return state_; }
  void set(T state, std::size_t time) {
    state_ = state;
    state_time_start_ = time;
  }
};

template<typename T, typename U>
inline T sum(const U& array) {
  T value{};
  for (auto& pt : array) {
    value += pt;
  }
  return value;
}

template<std::size_t log2_of_size, typename T = uint32_t>
class AveragingMeasurement {
  static constexpr std::size_t size(void) { return (1 << log2_of_size); }
  std::size_t index = 0;
  std::array<T, size()> data_{};

 public:
  void SetReading(const T data) {
    index++;
    data_[index&(size() - 1)] = data;
  }

  T GetSumOfLastNReadings(void) const {
    return sum<T>(data_);
  }

  T GetBufferAverage(void) const {
    return sum<T>(data_) >> log2_of_size;
  }

  T GetReading(void) const {
    return data_[index];
  }
};
}  //  namespace Utilities

#endif  //  UTILITIES_COMMONTYPES_H_
