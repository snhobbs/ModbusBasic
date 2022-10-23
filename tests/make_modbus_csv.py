import modbus_generator
import pandas

registers = (
    (
        "str20",
        "uint8_t",
        20,
        "input register",
    ),
    (
        "str",
        "uint8_t",
        4,
        "input register",
    ),
    (
        "int16",
        "uint32_t",
        1,
        "input register",
    ),
    (
        "int32",
        "uint32_t",
        1,
        "input register",
    ),
    (
        "int641",
        "uint64_t",
        1,
        "input register",
    ),
    (
        "int642",
        "uint64_t",
        1,
        "input register",
    ),
    (
        "int643",
        "uint64_t",
        1,
        "input register",
    ),
    (
        "int644",
        "uint64_t",
        1,
        "input register",
    ),
    (
        "int645",
        "uint64_t",
        1,
        "input register",
    ),
    (
        "int646",
        "uint64_t",
        1,
        "input register",
    ),
    (
        "int647",
        "uint64_t",
        1,
        "input register",
    ),
    (
        "int648",
        "uint64_t",
        1,
        "input register",
    ),
    (
        "int649",
        "uint64_t",
        1,
        "input register",
    ),
    (
        "int640",
        "uint64_t",
        1,
        "input register",
    ),
    (
        "int6411",
        "uint64_t",
        1,
        "input register",
    ),
    (
        "int6412",
        "uint64_t",
        1,
        "input register",
    ),
    (
        "int6413",
        "uint64_t",
        1,
        "input register",
    ),
    (
        "int6422",
        "uint64_t",
        1,
        "input register",
    ),
    (
        "str20",
        "uint8_t",
        20,
        "holding register",
    ),
    (
        "str",
        "uint8_t",
        4,
        "holding register",
    ),
    (
        "int16",
        "uint32_t",
        1,
        "holding register",
    ),
    (
        "int32",
        "uint32_t",
        1,
        "holding register",
    ),
    (
        "int641",
        "uint64_t",
        1,
        "holding register",
    ),
    (
        "int642",
        "uint64_t",
        1,
        "holding register",
    ),
    (
        "int643",
        "uint64_t",
        1,
        "holding register",
    ),
    (
        "int644",
        "uint64_t",
        1,
        "holding register",
    ),
    (
        "int645",
        "uint64_t",
        1,
        "holding register",
    ),
    (
        "int646",
        "uint64_t",
        1,
        "holding register",
    ),
    (
        "int647",
        "uint64_t",
        1,
        "holding register",
    ),
    (
        "int648",
        "uint64_t",
        1,
        "holding register",
    ),
    (
        "int649",
        "uint64_t",
        1,
        "holding register",
    ),
    (
        "int640",
        "uint64_t",
        1,
        "holding register",
    ),
    (
        "int6411",
        "uint64_t",
        1,
        "holding register",
    ),
    (
        "int6412",
        "uint64_t",
        1,
        "holding register",
    ),
    (
        "int6413",
        "uint64_t",
        1,
        "holding register",
    ),
    (
        "int6422",
        "uint64_t",
        1,
        "holding register",
    ),
)

if __name__ == "__main__":
    df = pandas.DataFrame(registers, columns=["name", "ctype", "length", "type"])
    df["data type"] = df["ctype"].replace("uint8_t", "string")
    df["register count"] = [
        modbus_generator.calculate_entry_registers(pt["data type"], pt["length"])
        for _, pt in df.iterrows()
    ]

    df["address"] = [0] * len(df)
    for function in df["type"].unique():
        indices = df["type"] == function
        entries = df[indices]
        addresses = modbus_generator.GetAddresses(
            [line for _, line in entries.iterrows()], starting=2, alignment=4
        )
        df.loc[indices, "address"] = addresses

    df.to_csv("modbus_map.csv")

    # modbus_generator.MakeHeader(GetHoldingRegisterMapEntries(), "TestHoldingRegisterMappedDataStore.h", name="HoldingRegisters")
    # modbus_generator.MakeHeader(GetInputRegisterMapEntries(), "TestInputRegisterMappedDataStore.h", name="InputRegisters")
