import copy
def SetAddresses(entries):
    address = 0
    for entry in entries:
        entry.address = address
        address += entry.registers

def GetInputMapEntries():
    MapEntries = (
        MapEntry('str20', DataType.uint8_t, 20, FunctionType.kInput),
        MapEntry('str', DataType.uint8_t, 4, FunctionType.kInput),
        MapEntry('int16', DataType.uint32_t, 1, FunctionType.kInput),
        MapEntry('int32', DataType.uint32_t, 1, FunctionType.kInput),
        MapEntry('int641', DataType.uint64_t, 1, FunctionType.kInput),
        MapEntry('int642', DataType.uint64_t, 1, FunctionType.kInput),
        MapEntry('int643', DataType.uint64_t, 1, FunctionType.kInput),
        MapEntry('int644', DataType.uint64_t, 1, FunctionType.kInput),
        MapEntry('int645', DataType.uint64_t, 1, FunctionType.kInput),
        MapEntry('int646', DataType.uint64_t, 1, FunctionType.kInput),
        MapEntry('int647', DataType.uint64_t, 1, FunctionType.kInput),
        MapEntry('int648', DataType.uint64_t, 1, FunctionType.kInput),
        MapEntry('int649', DataType.uint64_t, 1, FunctionType.kInput),
        MapEntry('int640', DataType.uint64_t, 1, FunctionType.kInput),
        MapEntry('int6411', DataType.uint64_t, 1, FunctionType.kInput),
        MapEntry('int6412', DataType.uint64_t, 1, FunctionType.kInput),
        MapEntry('int6413', DataType.uint64_t, 1, FunctionType.kInput),
        MapEntry('int6422', DataType.uint64_t, 1, FunctionType.kInput),
    )

    SetAddresses(MapEntries)
    return MapEntries

def GetOutputMapEntries():
    MapEntries = (
        MapEntry('str20', DataType.uint8_t, 20, FunctionType.kInput),
        MapEntry('str', DataType.uint8_t, 4, FunctionType.kHolding),
        MapEntry('int16', DataType.uint32_t, 1, FunctionType.kHolding),
        MapEntry('int32', DataType.uint32_t, 1, FunctionType.kHolding),
        MapEntry('int641', DataType.uint64_t, 1, FunctionType.kHolding),
        MapEntry('int642', DataType.uint64_t, 1, FunctionType.kHolding),
        MapEntry('int643', DataType.uint64_t, 1, FunctionType.kHolding),
        MapEntry('int644', DataType.uint64_t, 1, FunctionType.kHolding),
        MapEntry('int645', DataType.uint64_t, 1, FunctionType.kHolding),
        MapEntry('int646', DataType.uint64_t, 1, FunctionType.kHolding),
        MapEntry('int647', DataType.uint64_t, 1, FunctionType.kHolding),
        MapEntry('int648', DataType.uint64_t, 1, FunctionType.kHolding),
        MapEntry('int649', DataType.uint64_t, 1, FunctionType.kHolding),
        MapEntry('int640', DataType.uint64_t, 1, FunctionType.kHolding),
        MapEntry('int6411', DataType.uint64_t, 1, FunctionType.kHolding),
        MapEntry('int6412', DataType.uint64_t, 1, FunctionType.kHolding),
        MapEntry('int6413', DataType.uint64_t, 1, FunctionType.kHolding),
        MapEntry('int6422', DataType.uint64_t, 1, FunctionType.kHolding),
    )

    SetAddresses(MapEntries)
    return MapEntries

def Make(entries, template_dir, name, namespace):
    MakeHeader(template_dir, entries, name + ".h", namespace)

if __name__ == "__main__":
    template_dir = "/home/simon/software/Modbus/templates"
    import sys
    sys.path.append(template_dir)
    from MappedDataStore import *

    Make(GetInputMapEntries(), template_dir, name="TestInputRegisterMappedDataStore", namespace="InputRegisters")
    Make(GetOutputMapEntries(), template_dir, name="TestHoldingRegisterMappedDataStore", namespace="HoldingRegisters")

