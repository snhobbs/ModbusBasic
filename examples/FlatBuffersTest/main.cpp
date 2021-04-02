#include "LinuxSlave.h"
#include <cstring>
#include <cassert>
#include <cstdio>

int main(int argc, char* argv[]){

  HoldingRegisters holding_registers{};
  HoldingRegisterMemoryMap holding_register_data_map{&holding_registers};
  Modbus::MappedRegisterDataStore<HoldingRegisterMemoryMap> holding_register_data_store{&holding_register_data_map};
  HoldingRegisterController holding_registers_controller{&holding_register_data_store};

  InputRegisters input_registers{
	  std::array<const uint8_t, 64>{"string_0"}, std::array<const uint8_t, 64> {"string_1"}, std::array<const uint8_t, 64> {"string_2"}, std::array<const uint8_t, 40>{"string_3"},
	  1,2,3,4,5
  };
  InputRegisterMemoryMap input_map{&input_registers};
  Modbus::MappedRegisterDataStore<InputRegisterMemoryMap> input_register_data_store{&input_map};
  InputRegisterController input_register_controller{&input_register_data_store};

  uint8_t address = 246;
  if(argc < 2) {
    printf("Requires device port as argument, exiting\n");
    return 1;
  } if (argc > 2) {
    address = atoi(argv[2]);
    printf("Address: %d\n", address);
  }
  assert(argc >= 2);
  char dev_name[1024]{};
  printf("%s\n", argv[1]);
  strcpy(dev_name, argv[1]);


  LinuxSlave slave{argv[1], holding_registers_controller, input_register_controller};
  slave.SetAddress(address);
  std::size_t loops = 0;

  fflush(stdout);
  while(true) {
    loops++;
    slave.Run();
    sleep(0.05);
#if 0
    if (loops%(1<<10) == 0) {
      printf("[");
      const auto dstore = slave.hregs_.GetDataStore();
      for (std::size_t i = 0; i < dstore->size(); i++) {
        const uint16_t reg = dstore->GetRegister(i);
        if (reg != 0) {
          printf(" 0x%x", reg);
        }
      }
      printf("]\n");
    }
#endif
  }
  return 0;
}
