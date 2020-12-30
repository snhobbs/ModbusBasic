# Modbus
C++ Modbus library.

# Setting Up a Device

1. Device Identifier: define a DeviceIdentifier struct
with views of the identifiers.
2. Subclass of ProtocolRtuSlave.
  - Timeouts
  - When to process packets
  - IO device control, this layer puts the recieved characters 
    into the protocol and transmitting the generated responses
3. Data Stores
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

**Use** : socat -d -d -d -x PTY,link=/tmp/ptyp0,raw,echo=0,ispeed=9600 PTY,link=/tmp/ttyp0,raw,echo=0,ospeed=9600

In this example the pipes are setup at /tmp/ptyp0 and /tmp/ttyp0 running at 9600 baud with some debug info (level set by the number of '-d').
The master is started on one pipe and the slave on another and can be treated as connected hardware.

### Libmodbus

### PyModbus

[1]: http://www.dest-unreach.org/socat/
[2]: https://libmodbus.org/
[3]: https://github.com/Jacajack/liblightmodbus
[4]: https://github.com/riptideio/pymodbus
