from modbus_generator import (
    DataType,
    FunctionType,
    SetAddresses,
    MapEntry,
    ModbusRegister,
)
import modbus_generator

input_registers = (
    ModbusRegister("version", DataType.kString, 64, FunctionType.kInput),
    ModbusRegister("firmware_version", DataType.kString, 64, FunctionType.kInput),
    ModbusRegister("compile_date", DataType.kString, 64, FunctionType.kInput),
    ModbusRegister("compile_time", DataType.kString, 64, FunctionType.kInput),
    ModbusRegister("serial_number", DataType.kString, 40, FunctionType.kInput),
    ModbusRegister("fault_status", DataType.uint16_t, 1, FunctionType.kInput),
    ModbusRegister("p5_micro_volts", DataType.int32_t, 1, FunctionType.kInput),
    ModbusRegister("p5_reading", DataType.uint32_t, 1, FunctionType.kInput),
    ModbusRegister("p23_micro_volts", DataType.int32_t, 1, FunctionType.kInput),
    ModbusRegister("p23_reading", DataType.uint32_t, 1, FunctionType.kInput),
    ModbusRegister("vlo_micro_volts", DataType.int32_t, 1, FunctionType.kInput),
    ModbusRegister("vlo_reading", DataType.uint32_t, 1, FunctionType.kInput),
    ModbusRegister("thermistor_temp_spi", DataType.int32_t, 1, FunctionType.kInput),
    ModbusRegister("thermistor_temp_mcu", DataType.int32_t, 1, FunctionType.kInput),
    ModbusRegister("data_frequency", DataType.uint32_t, 1, FunctionType.kInput),
    ModbusRegister("p3_3_micro_volts", DataType.int32_t, 1, FunctionType.kInput),
    ModbusRegister("p3_3_reading", DataType.uint32_t, 1, FunctionType.kInput),
)

holding_registers = (
    ModbusRegister("analog_output0", DataType.uint32_t, 1, FunctionType.kHolding),
    ModbusRegister("analog_output1", DataType.uint32_t, 1, FunctionType.kHolding),
    ModbusRegister("isp_mode", DataType.uint32_t, 1, FunctionType.kHolding),
    ModbusRegister("slave_address_unlock", DataType.uint32_t, 1, FunctionType.kHolding),
    ModbusRegister("slave_address", DataType.uint16_t, 1, FunctionType.kHolding),
    ModbusRegister("clear_faults", DataType.uint32_t, 1, FunctionType.kHolding),
    ModbusRegister("start_data_read", DataType.uint32_t, 1, FunctionType.kHolding),
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
    modbus_generator.MakeHeader(
        GetHoldingRegisterMapEntries(),
        "HoldingRegisterMappedDataStore.h",
        name="HoldingRegisters",
    )
    modbus_generator.MakeHeader(
        GetInputRegisterMapEntries(),
        "InputRegisterMappedDataStore.h",
        name="InputRegisters",
    )

    modbus_generator.MakeLibModbusTest(
        input_registers=GetInputRegisterMapEntries(),
        holding_registers=GetHoldingRegisterMapEntries(),
        includes=["HoldingRegisterMappedDataStore.h", "InputRegisterMappedDataStore.h"],
    )

    modbus_generator.MakePyModbusTest(
        input_registers=GetInputRegisterMapEntries(),
        holding_registers=GetHoldingRegisterMapEntries(),
        includes=["HoldingRegisterMappedDataStore.h", "InputRegisterMappedDataStore.h"],
    )
