/*
 * Copyright 2020 ElectroOptical Innovations, LLC
 * */
#pragma once
#define ALIGNED(x) __attribute__((aligned(x)))
#include <array>
#include <cassert>

template <typename T, std::size_t kElements>
class LightWeightRingBuffer {
 private:
  static_assert((kElements > 0),
                "Size must be greater than 0 and a power of 2");
  static_assert(!(kElements & (kElements - 1)), "Size must be power of 2");
  //  static_assert(!(kElements*sizeof(T) % 4), "Size must be evenly distributed
  //  into 32bits to ensure alignment");
  std::array<T, kElements> buffer_ ALIGNED(32){};

  uint32_t head = 0;
  uint32_t tail = 0;

 protected:
  static uint32_t MaskIndex(const uint32_t Index) {
    uint32_t out = Index & (kElements - 1);
    return out;
  }

 public:
  uint32_t GetTail(void) const { return MaskIndex(tail); }
  uint32_t GetHead(void) const { return MaskIndex(head); }

  bool isEmpty(void) const { return head == tail; }
  bool isFull(void) const {
    return !isEmpty() && (GetHead() == GetTail());
  }

  std::size_t pop(T *const out, const std::size_t count) {
    std::size_t popped = 0;
    for (std::size_t i = 0; i < count; i++) {
      popped += pop(&out[i]);
    }
    return popped;
  }

  std::size_t pop(T *out) {
    if (!isEmpty()) {
      *out = buffer_[GetTail()];
      tail += 1;
      return 1;
    }
    return 0;
  }

  void peek(T *out, const std::size_t pos) const {
    //  pos is the count backwards from head, pos = 0 is head, pos = count is
    //  tail
    std::size_t ArrayPos = 0;
    if (pos <= GetCount()) {
      if (pos <= GetHead()) {
        ArrayPos = GetHead() - pos;
      } else {
        ArrayPos = GetTail() + (GetCount() - pos);
      }
    }
    *out = buffer_[ArrayPos];
  }


  std::size_t insert(const T *const in, const std::size_t count) {
    std::size_t inserted = 0;
    for (std::size_t i = 0; i < count; i++) {
      T entry = in[i];
      inserted += insert(entry);
    }
    return inserted;
  }

  std::size_t insert(const T &in) {
    if (!isFull()) {
      const uint32_t phead = GetHead();
      assert(phead < buffer_.size());
      buffer_.at(phead) = in;
      head += 1;
      return 1;
    }
    return 0;
  }

  std::size_t insertOverwrite(const T &in) {
    if (isFull()) {
      tail += 1;
    }
    return insert(in);
  }

  void reset(void) {
    head = 0;
    tail = 0;
  }

  std::size_t GetCount(void) const {
    std::size_t count = 0;
    if (tail <= head) {
      count = head - tail;
    } else {
      count = size() - tail + head;
    }
    assert(count <= kElements);
    return count;
  }

  static constexpr std::size_t size(void) { return kElements; }

  constexpr LightWeightRingBuffer(void) {}
  LightWeightRingBuffer(const LightWeightRingBuffer&) = delete;
  LightWeightRingBuffer operator=(const LightWeightRingBuffer&) = delete;
};

