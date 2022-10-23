/* Copyright (C) 2020 Electrooptical Innovations
 * ----------------------------------------------------------------------
 * Project:      Modbus
 * Title:        PosixSerial.h
 * Description:  Serial iodevice for posix system
 *
 * $Date:        13. May 2020
 * $Revision:    V.1.0.1
 * ----------------------------------------------------------------------
 */

#pragma once
#include <IODevice/IODevice.h>
#include <RingBuffer/RingBuffer.h>
#include <Utilities/TypeConversion.h>
#include <fcntl.h>  // File control definitions
#include <sys/time.h>
#include <termios.h>  // POSIX terminal control definitions
#include <unistd.h>   // UNIX standard function definitions

#include <cassert>
#include <cerrno>  // Error number definitions
#include <cstdint>
#include <cstdio>  // standard input / output functions
#include <cstdlib>
#include <cstring>  // string function definitions
#include <iostream>

int SetupSerial(const char *device, const speed_t baudrate = B9600) {
  const int flags = O_RDWR | O_NOCTTY | O_NDELAY | O_EXCL;
  int connection = open(device, flags);
  struct termios tty {};

  /* Error Handling */
  if (tcgetattr(connection, &tty) != 0) {
    std::cout << "Error " << errno << " from tcgetattr: " << strerror(errno)
              << std::endl;
  }

  /* Set Baud Rate */
  cfsetospeed(&tty, baudrate);
  cfsetispeed(&tty, baudrate);

  /* Setting other Port Stuff */
  tty.c_cflag &= ~PARENB;  // Make 8n1
  tty.c_cflag &= ~CSTOPB;
  tty.c_cflag &= ~CSIZE;
  tty.c_cflag |= CS8;

  tty.c_cflag &= ~CRTSCTS;  // no flow control
  // tty.c_cc[VMIN] = 1;            // one character minimum read
  tty.c_cc[VMIN] = 0;             // no character minimum
  tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout
  tty.c_cflag |= CREAD | CLOCAL;  // turn on READ & ignore ctrl lines

  /* Make raw */
  cfmakeraw(&tty);

  /* Flush Port, then applies attributes */
  tcflush(connection, TCIFLUSH);
  if (tcsetattr(connection, TCSANOW, &tty) != 0) {
    std::cout << "Error " << errno << " from tcsetattr" << std::endl;
  }
  return connection;
}

class UartController : public IODevice {
  const char *const device_name_;
  const speed_t baudrate_;
  int connection_ = 0;
  RingBuffer<uint8_t, 1024> rxbuff_;
  RingBuffer<uint8_t, 1024> txbuff_;

 public:
  virtual void Setup(void) {
    connection_ = SetupSerial(device_name_, baudrate_);
  }
  void sendByte(const uint8_t data) const { ::write(connection_, &data, 1); }
  int getByte(uint8_t &data) const { return ::read(connection_, &data, 1); }
  virtual uint32_t SendTxBuff(void) {
    int cnt = 0;
    while (!txbuff_.isEmpty()) {
      cnt++;
      uint8_t data = 0;
      txbuff_.pop(data);
      sendByte(data);
    }
    return cnt;
  }

  virtual uint32_t ReadIntoRxBuff(void) {
    uint8_t data = 0;
    int cnt = 0;
    while (getByte(data) == 1) {
      cnt++;
      rxbuff_.insert(data);
      data = 0;
    }
    return cnt;
  }
  UartController(const char *const device_name, const speed_t baudrate)
      : IODevice{rxbuff_, txbuff_},
        device_name_{device_name},
        baudrate_{baudrate} {
    Setup();
  }
};
