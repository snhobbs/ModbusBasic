#include <cassert>
#include <cstdio>
#include <cstring>

#include "LinuxSlave.h"
int main(int argc, char* argv[]) {
  if (argc < 2) {
    printf("Requires device port as argument, exiting\n");
    return 1;
  }
  assert(argc >= 2);
  char dev_name[1024]{};
  printf("%s\n", argv[1]);
  strcpy(dev_name, argv[1]);

  LinuxSlave slave{argv[1]};
  std::size_t loops = 0;
  while (true) {
    loops++;
    slave.Run();
    if (loops % (1 << 20) == 0) {
      printf("[");
      const auto dstore = slave.holding_register_data_store;
      for (std::size_t i = 0; i < dstore.size(); i++) {
        printf(" 0x%x", dstore.GetRegister(i));
      }
      printf("]\n");
    }
  }
  return 0;
}
