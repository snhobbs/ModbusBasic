import copy
from jinja2 import Template, Environment, FileSystemLoader
from modbus_generator import DataType, FunctionType, SetAddresses, Slave, MapEntry, ModbusRegister
import modbus_generator
import sys

input_registers = (
    ModbusRegister('str20', DataType.uint8_t, 20, FunctionType.kInput),
    ModbusRegister('str', DataType.uint8_t, 4, FunctionType.kInput),
    ModbusRegister('int16', DataType.uint32_t, 1, FunctionType.kInput),
    ModbusRegister('int32', DataType.uint32_t, 1, FunctionType.kInput),
    ModbusRegister('int641', DataType.uint64_t, 1, FunctionType.kInput),
    ModbusRegister('int642', DataType.uint64_t, 1, FunctionType.kInput),
    ModbusRegister('int643', DataType.uint64_t, 1, FunctionType.kInput),
    ModbusRegister('int644', DataType.uint64_t, 1, FunctionType.kInput),
    ModbusRegister('int645', DataType.uint64_t, 1, FunctionType.kInput),
    ModbusRegister('int646', DataType.uint64_t, 1, FunctionType.kInput),
    ModbusRegister('int647', DataType.uint64_t, 1, FunctionType.kInput),
    ModbusRegister('int648', DataType.uint64_t, 1, FunctionType.kInput),
    ModbusRegister('int649', DataType.uint64_t, 1, FunctionType.kInput),
    ModbusRegister('int640', DataType.uint64_t, 1, FunctionType.kInput),
    ModbusRegister('int6411', DataType.uint64_t, 1, FunctionType.kInput),
    ModbusRegister('int6412', DataType.uint64_t, 1, FunctionType.kInput),
    ModbusRegister('int6413', DataType.uint64_t, 1, FunctionType.kInput),
    ModbusRegister('int6422', DataType.uint64_t, 1, FunctionType.kInput),
)

holding_registers = (
    ModbusRegister('str20', DataType.uint8_t, 20, FunctionType.kInput),
    ModbusRegister('str', DataType.uint8_t, 4, FunctionType.kHolding),
    ModbusRegister('int16', DataType.uint32_t, 1, FunctionType.kHolding),
    ModbusRegister('int32', DataType.uint32_t, 1, FunctionType.kHolding),
    ModbusRegister('int641', DataType.uint64_t, 1, FunctionType.kHolding),
    ModbusRegister('int642', DataType.uint64_t, 1, FunctionType.kHolding),
    ModbusRegister('int643', DataType.uint64_t, 1, FunctionType.kHolding),
    ModbusRegister('int644', DataType.uint64_t, 1, FunctionType.kHolding),
    ModbusRegister('int645', DataType.uint64_t, 1, FunctionType.kHolding),
    ModbusRegister('int646', DataType.uint64_t, 1, FunctionType.kHolding),
    ModbusRegister('int647', DataType.uint64_t, 1, FunctionType.kHolding),
    ModbusRegister('int648', DataType.uint64_t, 1, FunctionType.kHolding),
    ModbusRegister('int649', DataType.uint64_t, 1, FunctionType.kHolding),
    ModbusRegister('int640', DataType.uint64_t, 1, FunctionType.kHolding),
    ModbusRegister('int6411', DataType.uint64_t, 1, FunctionType.kHolding),
    ModbusRegister('int6412', DataType.uint64_t, 1, FunctionType.kHolding),
    ModbusRegister('int6413', DataType.uint64_t, 1, FunctionType.kHolding),
    ModbusRegister('int6422', DataType.uint64_t, 1, FunctionType.kHolding),
)

def GetInputRegisterMapEntries():
    InputRegisterMapEntries = [MapEntry(register=pt) for pt in input_registers]
    SetAddresses(InputRegisterMapEntries)
    return InputRegisterMapEntries

def GetHoldingRegisterMapEntries():
    HoldingRegisterMapEntries = [MapEntry(register=pt) for pt in holding_registers]
    SetAddresses(HoldingRegisterMapEntries)
    return HoldingRegisterMapEntries

if __name__ == "__main__":
    modbus_generator.MakeHeader(GetHoldingRegisterMapEntries(), "TestHoldingRegisterMappedDataStore.h", namespace="HoldingRegisters")
    modbus_generator.MakeHeader(GetInputRegisterMapEntries(), "TestInputRegisterMappedDataStore.h", namespace="InputRegisters")

