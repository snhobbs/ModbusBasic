/*
 * Copyright 2020 ElectroOptical Innovations, LLC
 * */
#pragma once
#ifndef RINGBUFFER_RINGBUFFER_H_
#define RINGBUFFER_RINGBUFFER_H_

#define ALIGNED(x) __attribute__((aligned(x)))
#include <ArrayView/ArrayView.h>
#include <array>
#include <assert.h>
#include "LightweightRingBuffer.h"

template <typename T> class Buffer {
 public:
  virtual bool isEmpty(void) const = 0;
  virtual bool isFull(void) const = 0;
  virtual std::size_t pop(T *const out, const std::size_t count) = 0;
  virtual std::size_t insert(const T *const in, const std::size_t count) = 0;
  //  virtual std::size_t insert(const T& in) = 0;
  //  virtual std::size_t insert(T* const in, std::size_t count) {
  //  insert_const(in, count);
  virtual void Reset(void) = 0;
  virtual std::size_t GetCount(void) const = 0;
  virtual std::size_t Size(void) const = 0;

  Buffer(void) {}
  virtual ~Buffer(void) {}
};

template <typename T, std::size_t kElements>
class RingBuffer final : public Buffer<T> {
 public:
  virtual bool isEmpty(void) const { return buffer_.isEmpty(); }
  virtual bool isFull(void) const {
    return buffer_.isFull();
  }

  virtual std::size_t pop(T *out) {
    return buffer_.pop(out);
  }

  [[deprecated]] virtual std::size_t pop(T &out) { return pop(&out); }

  virtual std::size_t pop(T *const out, const std::size_t count) {
    return buffer_.pop(out, count);
  }

  virtual std::size_t insert(const T &in) {
    return buffer_.insert(in);
  }

  virtual std::size_t insert(const T *const in, const std::size_t count) {
    return buffer_.insert(in, count);
  }

  std::size_t insertOverwrite(const T &in) {
    return buffer_.insertOverwrite(in);
  }

  void peek(T *out, const std::size_t pos) const {
    buffer_.peek(out, pos);
  }

  virtual void Reset(void) {
    buffer_.reset();
  }

  [[deprecated]] void reset() { Reset(); }

  virtual std::size_t GetCount(void) const {
    return buffer_.GetCount();
  }
  static constexpr std::size_t GetSize(void) { return kElements; }
  static constexpr std::size_t frameSize(void) { return GetSize(); }

  virtual std::size_t Size(void) const { return GetSize(); }
  std::size_t size(void) const { return GetSize(); }

  virtual ~RingBuffer(void) {}
  constexpr RingBuffer(void) {}
  RingBuffer(const RingBuffer &) = delete;
  RingBuffer operator=(const RingBuffer &) = delete;

 private:
  LightWeightRingBuffer<T, kElements> buffer_;
};

#endif //  RINGBUFFER_RINGBUFFER_H_
