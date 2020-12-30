/*
 *
 * Copyright 2020 ElectroOptical Innovations, LLC
 * */
#pragma once
#ifndef ARRAYVIEW_ARRAYVIEW_H_
#define ARRAYVIEW_ARRAYVIEW_H_
#include <cassert>

#if 0
#include <string_view>
template<typename T>
class ArrayView : public std::basic_string_view<T>{
public:
  ArrayView(std::size_t length, T* pointer) :
    std::basic_string_view<T>{length, pointer} {
      assert(length >= 0);
  }
};
#else
template<typename T>
class ArrayView {
    const std::size_t length_;
    T* const pointer_;

 public:
    [[deprecated]]
    std::size_t Size(void) const { return length_; }
    std::size_t size(void) const { return length_; }
    T& at(std::size_t index) {
        assert(index <= size());
        return pointer_[index < size() ? index : 0];
    }
    const T& at(std::size_t index) const {
        assert(index <= size());
        return pointer_[index < size() ? index : 0];
    }
    const T& operator[](std::size_t index) const {
        return at(index);
    }
    T& operator[](std::size_t index) {
        assert(index <= size());
        return pointer_[index < size() ? index : 0];
    }
    const T* begin(void) const {
        return pointer_;
    }
    const T* end(void) const {
        return begin() + size();
    }
    T* begin(void) {
        return pointer_;
    }
    T* end(void) {
        return begin() + size();
    }
    const T* data(void) const noexcept {
        return begin();
    }
    T* data(void) noexcept {
        return begin();
    }

    ArrayView(std::size_t length, T* pointer) :
        length_{length}, pointer_{pointer} {
            assert(length >= 0);
        }
};
#endif

#endif  //  ARRAYVIEW_ARRAYVIEW_H_
