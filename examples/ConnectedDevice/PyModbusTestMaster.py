import sys
from time import sleep
from pymodbus.client.sync import ModbusSerialClient as ModbusClient

import sys
from modbus_generator import DataType

sleep_time = 0
kModbusAddress = 1

def RegistersToBytes(registers):
  text = []
  for register in registers:
      text.append((register>>8)&0xff)
      text.append((register&0xff))
  return bytes(text)

def DecodeText(data: list):
    return RegistersToBytes(data).decode("utf-8")

def TransformBytesToDataType(dtype : DataType, data : bytes, byteorder="big"):
    if(dtype == DataType.int32_t):
        return int.from_bytes(data, byteorder=byteorder, signed=True)
    elif(dtype == DataType.uint32_t):
        return int.from_bytes(data, byteorder=byteorder, signed=False)
    elif(dtype == DataType.int16_t):
        return int.from_bytes(data, byteorder=byteorder, signed=True)
    elif(dtype == DataType.uint16_t):
        return int.from_bytes(data, byteorder=byteorder, signed=False)
    elif(dtype == DataType.int8_t):
        return int.from_bytes(data, byteorder=byteorder, signed=True)
    elif(dtype == DataType.uint8_t):
        return int.from_bytes(data, byteorder=byteorder, signed=False)
    elif(dtype == DataType.kString):
        return data.decode("utf-8")

class ModbusMaster:
    parity = 'N'
    bytesize = 8
    stopbits = 1
    method= "rtu"
    timeout_seconds = .1

    def __init__(self, port, baud = 9600):
        self.port = port
        self.baudrate = baud
        #method=self.method,
        self.client = ModbusClient(method=self.method, port=self.port, parity=self.parity,
            timeout=self.timeout_seconds, bytesize=self.bytesize,
            stopbits=self.stopbits, baudrate=self.baudrate)
        self.connect()

    def disconnect(self):
        self.client.close()

    def connect(self):
        self.client.connect()
        print("Connection Successful")

def ReadInputRegister_version(master):
    address = 0
    registers = 32
    response = master.client.read_input_registers(address, registers, unit=kModbusAddress)
    data_bytes = RegistersToBytes(response.registers)
    # Transform to correct type
    value = TransformBytesToDataType(DataType.kString, data_bytes)
    return value

def ReadInputRegister_firmware_version(master):
    address = 32
    registers = 32
    response = master.client.read_input_registers(address, registers, unit=kModbusAddress)
    data_bytes = RegistersToBytes(response.registers)
    # Transform to correct type
    value = TransformBytesToDataType(DataType.kString, data_bytes)
    return value

def ReadInputRegister_compile_date(master):
    address = 64
    registers = 32
    response = master.client.read_input_registers(address, registers, unit=kModbusAddress)
    data_bytes = RegistersToBytes(response.registers)
    # Transform to correct type
    value = TransformBytesToDataType(DataType.kString, data_bytes)
    return value

def ReadInputRegister_compile_time(master):
    address = 96
    registers = 32
    response = master.client.read_input_registers(address, registers, unit=kModbusAddress)
    data_bytes = RegistersToBytes(response.registers)
    # Transform to correct type
    value = TransformBytesToDataType(DataType.kString, data_bytes)
    return value

def ReadInputRegister_serial_number(master):
    address = 128
    registers = 20
    response = master.client.read_input_registers(address, registers, unit=kModbusAddress)
    data_bytes = RegistersToBytes(response.registers)
    # Transform to correct type
    value = TransformBytesToDataType(DataType.kString, data_bytes)
    return value

def ReadInputRegister_fault_status(master):
    address = 148
    registers = 1
    response = master.client.read_input_registers(address, registers, unit=kModbusAddress)
    data_bytes = RegistersToBytes(response.registers)
    # Transform to correct type
    value = TransformBytesToDataType(DataType.uint16_t, data_bytes)
    return value

def ReadInputRegister_p5_micro_volts(master):
    address = 150
    registers = 2
    response = master.client.read_input_registers(address, registers, unit=kModbusAddress)
    data_bytes = RegistersToBytes(response.registers)
    # Transform to correct type
    value = TransformBytesToDataType(DataType.int32_t, data_bytes)
    return value

def ReadInputRegister_p5_reading(master):
    address = 152
    registers = 2
    response = master.client.read_input_registers(address, registers, unit=kModbusAddress)
    data_bytes = RegistersToBytes(response.registers)
    # Transform to correct type
    value = TransformBytesToDataType(DataType.uint32_t, data_bytes)
    return value

def ReadInputRegister_p23_micro_volts(master):
    address = 154
    registers = 2
    response = master.client.read_input_registers(address, registers, unit=kModbusAddress)
    data_bytes = RegistersToBytes(response.registers)
    # Transform to correct type
    value = TransformBytesToDataType(DataType.int32_t, data_bytes)
    return value

def ReadInputRegister_p23_reading(master):
    address = 156
    registers = 2
    response = master.client.read_input_registers(address, registers, unit=kModbusAddress)
    data_bytes = RegistersToBytes(response.registers)
    # Transform to correct type
    value = TransformBytesToDataType(DataType.uint32_t, data_bytes)
    return value

def ReadInputRegister_vlo_micro_volts(master):
    address = 158
    registers = 2
    response = master.client.read_input_registers(address, registers, unit=kModbusAddress)
    data_bytes = RegistersToBytes(response.registers)
    # Transform to correct type
    value = TransformBytesToDataType(DataType.int32_t, data_bytes)
    return value

def ReadInputRegister_vlo_reading(master):
    address = 160
    registers = 2
    response = master.client.read_input_registers(address, registers, unit=kModbusAddress)
    data_bytes = RegistersToBytes(response.registers)
    # Transform to correct type
    value = TransformBytesToDataType(DataType.uint32_t, data_bytes)
    return value

def ReadInputRegister_thermistor_temp_spi(master):
    address = 162
    registers = 2
    response = master.client.read_input_registers(address, registers, unit=kModbusAddress)
    data_bytes = RegistersToBytes(response.registers)
    # Transform to correct type
    value = TransformBytesToDataType(DataType.int32_t, data_bytes)
    return value

def ReadInputRegister_thermistor_temp_mcu(master):
    address = 164
    registers = 2
    response = master.client.read_input_registers(address, registers, unit=kModbusAddress)
    data_bytes = RegistersToBytes(response.registers)
    # Transform to correct type
    value = TransformBytesToDataType(DataType.int32_t, data_bytes)
    return value

def ReadInputRegister_data_frequency(master):
    address = 166
    registers = 2
    response = master.client.read_input_registers(address, registers, unit=kModbusAddress)
    data_bytes = RegistersToBytes(response.registers)
    # Transform to correct type
    value = TransformBytesToDataType(DataType.uint32_t, data_bytes)
    return value

def ReadInputRegister_p3_3_micro_volts(master):
    address = 168
    registers = 2
    response = master.client.read_input_registers(address, registers, unit=kModbusAddress)
    data_bytes = RegistersToBytes(response.registers)
    # Transform to correct type
    value = TransformBytesToDataType(DataType.int32_t, data_bytes)
    return value

def ReadInputRegister_p3_3_reading(master):
    address = 170
    registers = 2
    response = master.client.read_input_registers(address, registers, unit=kModbusAddress)
    data_bytes = RegistersToBytes(response.registers)
    # Transform to correct type
    value = TransformBytesToDataType(DataType.uint32_t, data_bytes)
    return value



def ReadHoldingRegister_analog_output0(master):
    address = 0
    registers = 2
    response = master.client.read_holding_registers(address, registers, unit=kModbusAddress)
    data_bytes = RegistersToBytes(response.registers)
    value = TransformBytesToDataType(DataType.uint32_t, data_bytes)
    return value

def WriteHoldingRegister_analog_output0(master, register_values):
    registers = 2
    assert(len(register_values) == registers)
    response = master.client.write_registers(0, register_values, unit=kModbusAddress)
    print("analog_output0 Response:", response)

def ReadHoldingRegister_analog_output1(master):
    address = 2
    registers = 2
    response = master.client.read_holding_registers(address, registers, unit=kModbusAddress)
    data_bytes = RegistersToBytes(response.registers)
    value = TransformBytesToDataType(DataType.uint32_t, data_bytes)
    return value

def WriteHoldingRegister_analog_output1(master, register_values):
    registers = 2
    assert(len(register_values) == registers)
    response = master.client.write_registers(2, register_values, unit=kModbusAddress)
    print("analog_output1 Response:", response)

def ReadHoldingRegister_isp_mode(master):
    address = 4
    registers = 2
    response = master.client.read_holding_registers(address, registers, unit=kModbusAddress)
    data_bytes = RegistersToBytes(response.registers)
    value = TransformBytesToDataType(DataType.uint32_t, data_bytes)
    return value

def WriteHoldingRegister_isp_mode(master, register_values):
    registers = 2
    assert(len(register_values) == registers)
    response = master.client.write_registers(4, register_values, unit=kModbusAddress)
    print("isp_mode Response:", response)

def ReadHoldingRegister_slave_address_unlock(master):
    address = 6
    registers = 2
    response = master.client.read_holding_registers(address, registers, unit=kModbusAddress)
    data_bytes = RegistersToBytes(response.registers)
    value = TransformBytesToDataType(DataType.uint32_t, data_bytes)
    return value

def WriteHoldingRegister_slave_address_unlock(master, register_values):
    registers = 2
    assert(len(register_values) == registers)
    response = master.client.write_registers(6, register_values, unit=kModbusAddress)
    print("slave_address_unlock Response:", response)

def ReadHoldingRegister_slave_address(master):
    address = 8
    registers = 1
    response = master.client.read_holding_registers(address, registers, unit=kModbusAddress)
    data_bytes = RegistersToBytes(response.registers)
    value = TransformBytesToDataType(DataType.uint16_t, data_bytes)
    return value

def WriteHoldingRegister_slave_address(master, register_values):
    registers = 1
    assert(len(register_values) == registers)
    response = master.client.write_registers(8, register_values, unit=kModbusAddress)
    print("slave_address Response:", response)

def ReadHoldingRegister_clear_faults(master):
    address = 10
    registers = 2
    response = master.client.read_holding_registers(address, registers, unit=kModbusAddress)
    data_bytes = RegistersToBytes(response.registers)
    value = TransformBytesToDataType(DataType.uint32_t, data_bytes)
    return value

def WriteHoldingRegister_clear_faults(master, register_values):
    registers = 2
    assert(len(register_values) == registers)
    response = master.client.write_registers(10, register_values, unit=kModbusAddress)
    print("clear_faults Response:", response)

def ReadHoldingRegister_start_data_read(master):
    address = 12
    registers = 2
    response = master.client.read_holding_registers(address, registers, unit=kModbusAddress)
    data_bytes = RegistersToBytes(response.registers)
    value = TransformBytesToDataType(DataType.uint32_t, data_bytes)
    return value

def WriteHoldingRegister_start_data_read(master, register_values):
    registers = 2
    assert(len(register_values) == registers)
    response = master.client.write_registers(12, register_values, unit=kModbusAddress)
    print("start_data_read Response:", response)



if __name__ == "__main__":
    port = "/tmp/ptyp0"
    kModbusAddress = 1
    if len(sys.argv) > 1:
        port = sys.argv[1]
    if len(sys.argv) > 2:
        kModbusAddress = int(sys.argv[2])
    print("Device: %s"%port)
    print("Address: %d"%kModbusAddress)
    master = ModbusMaster(port, 9600)
    print("%s\nReading Input Registers\n%s"%("#"*80, "#"*80))


    value = ReadInputRegister_version(master)
    print("version: {}\n".format(value))
    sleep(sleep_time)

    value = ReadInputRegister_firmware_version(master)
    print("firmware_version: {}\n".format(value))
    sleep(sleep_time)

    value = ReadInputRegister_compile_date(master)
    print("compile_date: {}\n".format(value))
    sleep(sleep_time)

    value = ReadInputRegister_compile_time(master)
    print("compile_time: {}\n".format(value))
    sleep(sleep_time)

    value = ReadInputRegister_serial_number(master)
    print("serial_number: {}\n".format(value))
    sleep(sleep_time)

    value = ReadInputRegister_fault_status(master)
    print("fault_status: {}\n".format(value))
    sleep(sleep_time)

    value = ReadInputRegister_p5_micro_volts(master)
    print("p5_micro_volts: {}\n".format(value))
    sleep(sleep_time)

    value = ReadInputRegister_p5_reading(master)
    print("p5_reading: {}\n".format(value))
    sleep(sleep_time)

    value = ReadInputRegister_p23_micro_volts(master)
    print("p23_micro_volts: {}\n".format(value))
    sleep(sleep_time)

    value = ReadInputRegister_p23_reading(master)
    print("p23_reading: {}\n".format(value))
    sleep(sleep_time)

    value = ReadInputRegister_vlo_micro_volts(master)
    print("vlo_micro_volts: {}\n".format(value))
    sleep(sleep_time)

    value = ReadInputRegister_vlo_reading(master)
    print("vlo_reading: {}\n".format(value))
    sleep(sleep_time)

    value = ReadInputRegister_thermistor_temp_spi(master)
    print("thermistor_temp_spi: {}\n".format(value))
    sleep(sleep_time)

    value = ReadInputRegister_thermistor_temp_mcu(master)
    print("thermistor_temp_mcu: {}\n".format(value))
    sleep(sleep_time)

    value = ReadInputRegister_data_frequency(master)
    print("data_frequency: {}\n".format(value))
    sleep(sleep_time)

    value = ReadInputRegister_p3_3_micro_volts(master)
    print("p3_3_micro_volts: {}\n".format(value))
    sleep(sleep_time)

    value = ReadInputRegister_p3_3_reading(master)
    print("p3_3_reading: {}\n".format(value))
    sleep(sleep_time)


    print("%s\nWriting/Reading Holding Registers\n%s"%("#"*80, "#"*80))


    analog_output0_write_values = list(range(2))
    response = WriteHoldingRegister_analog_output0(master, analog_output0_write_values)
    value = ReadHoldingRegister_analog_output0(master)
    print("analog_output0: {}\n".format(value))
    #assert(analog_output0_write_values == response.registers)


    analog_output1_write_values = list(range(2))
    response = WriteHoldingRegister_analog_output1(master, analog_output1_write_values)
    value = ReadHoldingRegister_analog_output1(master)
    print("analog_output1: {}\n".format(value))
    #assert(analog_output1_write_values == response.registers)


    isp_mode_write_values = list(range(2))
    response = WriteHoldingRegister_isp_mode(master, isp_mode_write_values)
    value = ReadHoldingRegister_isp_mode(master)
    print("isp_mode: {}\n".format(value))
    #assert(isp_mode_write_values == response.registers)


    slave_address_unlock_write_values = list(range(2))
    response = WriteHoldingRegister_slave_address_unlock(master, slave_address_unlock_write_values)
    value = ReadHoldingRegister_slave_address_unlock(master)
    print("slave_address_unlock: {}\n".format(value))
    #assert(slave_address_unlock_write_values == response.registers)


    slave_address_write_values = list(range(1))
    response = WriteHoldingRegister_slave_address(master, slave_address_write_values)
    value = ReadHoldingRegister_slave_address(master)
    print("slave_address: {}\n".format(value))
    #assert(slave_address_write_values == response.registers)


    clear_faults_write_values = list(range(2))
    response = WriteHoldingRegister_clear_faults(master, clear_faults_write_values)
    value = ReadHoldingRegister_clear_faults(master)
    print("clear_faults: {}\n".format(value))
    #assert(clear_faults_write_values == response.registers)


    start_data_read_write_values = list(range(2))
    response = WriteHoldingRegister_start_data_read(master, start_data_read_write_values)
    value = ReadHoldingRegister_start_data_read(master)
    print("start_data_read: {}\n".format(value))
    #assert(start_data_read_write_values == response.registers)


    master.disconnect()
