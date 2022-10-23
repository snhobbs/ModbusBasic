# Modbus
C++ Modbus parsing library.

# Setting Up a Slave Device
The focus of this library is to allow for a serializable data types to be exposed over
modbus in a clearer manner. Modbus registers are unsigned 16 bit values. Other data types
need to be translated to these 16 bit values and back. The idea of this library is to use
a user defined data store type that controls access to the underlying data types this is especially
useful for preventing unaligned data accesses.

A slave device is composed of controlled and data stores using C++ templates.
Each slave needs a CoilController, HoldingRegisterController, DiscreteInputController, and an InputRegisterController.
Each of these controls access to a data store.
A simple data store for registers would just be an array of uint16_ts. For a coil or discrete input it would be a bit field.
These basic data stores are available but are less powerful than creating a data store for the data types being used.
The basic controller types are:

  using CoilController =
      Modbus::CoilController<Modbus::BitFieldDataStore<kCoilCount>>;
  using HoldingRegisterController =
    Modbus::HoldingRegisterController<Modbus::RegisterDataStore<kHoldingRegisterCount>>;
  using DiscreteInputController =
      Modbus::DiscreteInputController<Modbus::BitFieldDataStore<kDiscreteInputCount>>;
  using InputRegisterController =
      Modbus::InputRegisterController<Modbus::RegisterDataStore<kInputRegisterCount>>;

## Specialized Data Stores
Using mapped data stores allows the user to expose the actual data types being used with
the serialization encapsulated.

using HoldingRegisterController =
    Modbus::HoldingRegisterController<Modbus::MappedRegisterDataStore<HoldingRegisters::MemoryMapController>>;
using InputRegisterController =
    Modbus::InputRegisterController<Modbus::MappedRegisterDataStore<InputRegisters::MemoryMapController>>;


1. Subclass of ProtocolRtuSlave.
  - Timeouts
  - When to process packets
  - IO device control, this layer puts the recieved characters
    into the protocol and transmitting the generated responses
2. Data Stores
  - The holding registers, coils, discrete inputs, and input registers are
  generalized as data stores. The default store mechanism can be used where
  the data store is literal, i.e. all bits and registers are stored
  and accessed directly. Other data stores might define a memory map
  that accesses system variables. This will lower memory use and remove the
  need to update the store or poll on changes.

## Testing With Other Libraries
### Using socat
[socat][1] is an easy way to spoof a serial
connection. A bidirectional pipe is setup allowing two processes
to see the pipe as if it was an attached serial device.

**Use** :
socat -d -d -d -x PTY,link=/tmp/ptyp0,raw,echo=0,ispeed=b9600 PTY,link=/tmp/ttyp0,raw,echo=0,ospeed=b9600

In this example the pipes are setup at /tmp/ptyp0 and /tmp/ttyp0 running at 9600 baud with some debug info (level set by the number of '-d').
The master is started on one pipe and the slave on another and can be treated as connected hardware.

### Libmodbus

### PyModbus

[1]: http://www.dest-unreach.org/socat/
[2]: https://libmodbus.org/
[3]: https://github.com/Jacajack/liblightmodbus
[4]: https://github.com/riptideio/pymodbus
