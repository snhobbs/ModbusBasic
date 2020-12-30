/*
 * Copyright 2020 ElectroOptical Innovations, LLC
 * */

#pragma once
#ifndef IODEVICE_IODEVICE_H_
#define IODEVICE_IODEVICE_H_

#include <RingBuffer/RingBuffer.h>
#include <cstdint>

#if 0
class IODevice{
 public:
    virtual int read(uint8_t* buff, const std::size_t len) = 0;
    virtual std::size_t rxPop(uint8_t& ch) = 0;
    virtual bool rxEmpty(void) const = 0;
    virtual bool txEmpty(void) const = 0;
    virtual std::size_t write(const uint8_t* const buff, const std::size_t len) = 0;
    void write(uint8_t chr) {
        write(&chr, 1);
    }

    virtual void reset() = 0;
    virtual void setup() = 0;

    constexpr IODevice(void) {}
    virtual ~IODevice(void) {}
};
#endif

class IODevice {
  Buffer<uint8_t> &rxbuff_;
  Buffer<uint8_t> &txbuff_;

 public:
  std::size_t read(uint8_t *buff, const std::size_t len) {
    std::size_t bytes = rxbuff_.pop(buff, len);
    return bytes;
  }

  std::size_t RxPop(uint8_t *ch) {
    return rxbuff_.pop(ch, 1);
  }

  [[deprecated]]
  std::size_t rxPop(uint8_t &ch) { return RxPop(&ch); }

  bool RxEmpty(void) const {
    return rxbuff_.isEmpty();
  }

  [[deprecated]]
  bool rxEmpty(void) const { return RxEmpty(); }

  bool TxEmpty(void) const {
    return txbuff_.isEmpty();
  }

  [[deprecated]] bool txEmpty(void) const { return TxEmpty(); }


  std::size_t write(const uint8_t &chr) { return write(&chr, 1); }
  std::size_t write(const uint8_t *const buff, const std::size_t len) {
    //  Fill txbuff_
    std::size_t bytes = txbuff_.insert(buff, len);
    return bytes;
  }

  void reset(void) {
    rxbuff_.Reset();
    txbuff_.Reset();
  }

  constexpr IODevice(Buffer<uint8_t> &r_rxbuff, Buffer<uint8_t> &r_txbuff)
      : rxbuff_{r_rxbuff}, txbuff_{r_txbuff} {}
  IODevice(const IODevice &) = delete;
  IODevice operator=(const IODevice &) = delete;
};

#endif //  IODEVICE_IODEVICE_H_
