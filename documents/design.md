## Components
+ There are two types of controller
    + Bit : coil, input
    + Register : input register, holding register
+ The controllers support:
    + read
    + write
    + access valid
+ These are passed to an Accessor schema object to control the interface
+ The schema handles the requests:
    + read : start, length, buffer -> reads data from the controller
    + write : start, length, buffer -> writes data to the controller
    + access valid : start, length -> returns whether or not the location can be read/written to
+ The schema is passed to a protocol controller which handles all packets
    + Data is available immediately for both writes and reads

+ Read & writes of controllers are done with a start, length, buffer, u8Buffer


## Interface with code
The above section describes how the memory blocks are interfaced over the modbus network.
This data needs to also be read and written to from the main line code. Data types need
to be mapped to and from uint8_t*. This could mean a serialize and deserialize per data type.
These can be template specializations. All read/writes are done through the serialize.
Using the serialize all the information about the specific entry can be stored in data controller.
Write functions to handle this:
+ write_as_type<T>(uint8_t* storage, const size_t storage_length, const T* data, const size_t data_length)
+ read_as_type<T>(const uint8_t* storage, const size_t storage_length, T* data, const size_t data_length)

A data controller could wrap these functions to give an interface to the code where they would not have any details
about the code.

struct u8_Buffer {
  uint8_t* buffer;
  size_t size;
};

struct BitField {
  size_t address;
  size_t length;
  u8_Buffer* buffer;
};

typedef BitField RegisterField;

## Components
+ BitController{BitField* entries, const size_t entry_length}
+ RegisterController{RegisterField* entries, const size_t entry_length}
+ Accessor{BitController* p_coils, BitController* p_discrete_inputs, RegisterController* p_holding_registers, RegisterController* p_input_registers}
	Combines the different fields exposing the same read, write, validate functions
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
    + should call T deserialize<T>(uint8_t* storage, const size_t storage_length)
+ read_as_type<T>(const uint8_t* storage, const size_t storage_length, T* data, const size_t data_length)
    + should call void serialize<T>(uint8_t* storage, const size_t storage_length, const T& object)

+ Bit read and write are done with an array of bytes, and a write length. A buffer length is also passed for checking.
Each controller is initialized by passing it a linked list of data objects.
The protocol controller and controllers are fully put in a library.



# Protocol Controller
+ reads, writes passed through accessor
+ wether or not a access is valid is handled by accessor
+ Bytes come in, they should be passed consistantly to the reader, updating the state
  + Packet full, all data read
  + Check crc, reject send error response or continue
  + Find action and field type from packet header, if action is unsupported send error else continue
      +  FieldType GetAddressSpaceFromFunction(Command)
  + validate access through accessor, send error or continue
  + Apply action through accessor
  + Calculate crc
  + send packet
+ Data is sent directly to uart, uart buffer should be large enough to handle the largest access

+ ReadStateMachine
  + Knows the type of packets, applies logic to know state
    + Read known header length till packet type
    + Request meta data length
    + read till meta data finished
        + Interfaces? Polymorphic handling
    + request data length
        + Interface, needs to call something to process the packet.
    + read till data finished
    + dispatch
+ Crc Calculator
+ PacketReader
  + Structure knowlege shared with ReadStateMachine, shared knowledge should be factored out
+ Interface to IO
