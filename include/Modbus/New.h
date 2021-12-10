/*
+ BitController{BitField* entries, const size_t entry_length}
  + access valid
  + read
  + write
+ RegisterController{RegisterField* entries, const size_t entry_length}
+ ModbusSchema{BitController* p_coils, BitController* p_discrete_inputs, RegisterController* p_holding_registers, RegisterController* p_input_registers}
+ ProtocolController{IODevice* iodevice, ModbusSchema* p_schema}
    + Should be broken into slave/master to reduce dependancies
    + Both have their update functions called with the current time for rough timing between packets
      + Timing between packets could be enforced with a delay value pushed to the IODevice
    + Right now this is only supporting servers (slaves)
+ DataController{} -> contains and passes the Bit and RegisterFields to the controllers.
  + read_<Entry Name>()
  + write_<Entry Name>(const T& entry)

## Functions
+ write_as_type<T>(uint8_t* storage, const size_t storage_length, const T* data, const size_t data_length)
    + should call void serialize<T>(uint8_t* storage, const size_t storage_length, const T& object)
+ read_as_type<T>(const uint8_t* storage, const size_t storage_length, T* data, const size_t data_length)
    + should call T deserialize<T>(uint8_t* storage, const size_t storage_length)
*/
#pragma once
#include <cstddef>
#include <cstdint>
#include <cassert>
#include <cstring>
#include <climits>
#include <algorithm>
#include "Modbus/ModbusNew.h"
#include "Modbus/BitController.h"
#include "Modbus/RegisterController.h"


