# Modbus
C++ Modbus parsing library.

## Setting Up a Slave Device
This library facilitates exposing serializable data types over Modbus in a more transparent manner. Modbus registers are 16-bit unsigned integers, and other data types must be translated into these 16-bit values and vice versa. The core concept of the library is to use a user-defined data store type that controls access to the underlying data types. This approach is especially useful for preventing unaligned data access.

A slave device in this library consists of controlled data stores, managed via C++ templates. Each slave device requires a `CoilController`, `HoldingRegisterController`, `DiscreteInputController`, and an `InputRegisterController`, with each controller managing access to a specific data store.

A basic data store for registers might simply be an array of `uint16_t` values, while coils or discrete inputs are typically stored as bit fields. Basic data stores are available, but more complex, user-defined data stores can provide additional flexibility and power.

Example of basic controller types:

```cpp
using CoilController =
    Modbus::CoilController<Modbus::BitFieldDataStore<kCoilCount>>;
using HoldingRegisterController =
    Modbus::HoldingRegisterController<Modbus::RegisterDataStore<kHoldingRegisterCount>>;
using DiscreteInputController =
    Modbus::DiscreteInputController<Modbus::BitFieldDataStore<kDiscreteInputCount>>;
using InputRegisterController =
    Modbus::InputRegisterController<Modbus::RegisterDataStore<kInputRegisterCount>>;
````

### Specialized Data Stores

To expose actual data types with the serialization encapsulated, you can use mapped data stores. For instance:

```cpp
using HoldingRegisterController =
    Modbus::HoldingRegisterController<Modbus::MappedRegisterDataStore<HoldingRegisters::MemoryMapController>>;
using InputRegisterController =
    Modbus::InputRegisterController<Modbus::MappedRegisterDataStore<InputRegisters::MemoryMapController>>;
```

## Protocol Layer

A slave device is managed by a subclass of `ProtocolRtuSlave`. The protocol layer handles:

1. **Timeouts**: Managing the timeout between the master and slave.
2. **Packet Processing**: Determining when to process incoming packets.
3. **IO Device Control**: This layer receives characters and inputs them into the protocol, while also handling the transmission of generated responses.

## Data Stores

The Modbus data types—holding registers, coils, discrete inputs, and input registers—are treated as data stores. By default, the data store mechanism directly stores and accesses all bits and registers. However, it is possible to use specialized data stores that define a memory map to access system variables, which reduces memory usage and eliminates the need for periodic updates or polling.

## Testing With Other Libraries

### Using socat

[socat](http://www.dest-unreach.org/socat/) can simulate a serial connection between two processes. It creates a bidirectional pipe, allowing both processes to treat the pipe as if it were a physical serial device.

**Example**:

```bash
socat -d -d -d -x PTY,link=/tmp/ptyp0,raw,echo=0,ispeed=b9600 PTY,link=/tmp/ttyp0,raw,echo=0,ospeed=b9600
```

This command sets up two pipes at `/tmp/ptyp0` and `/tmp/ttyp0` running at 9600 baud with debug information. The master can be started on one pipe, and the slave on another, simulating a connected hardware setup.

### Libmodbus

For more information on the Modbus protocol and a C library implementation, check out [Libmodbus](https://libmodbus.org/).

### PyModbus

PyModbus is a Python library that implements the Modbus protocol. It can be used to test and interface with the C++ slave. More information can be found on [GitHub](https://github.com/riptideio/pymodbus).

## References

+ [socat](http://www.dest-unreach.org/socat/)
+ [Libmodbus](https://libmodbus.org/)
+ [PyModbus GitHub](https://github.com/riptideio/pymodbus)
+ [liblightmodbus](https://github.com/Jacajack/liblightmodbus)
