#include <cassert>
#include <cstdio>
#include <cstring>

#include "LinuxSlave.h"
int main(int argc, char* argv[]) {
  uint8_t address = 246;
  if (argc < 2) {
    printf("Requires device port as argument, exiting\n");
    return 1;
  }
  if (argc > 2) {
    address = atoi(argv[2]);
    printf("Address: %d\n", address);
  }
  assert(argc >= 2);
  char dev_name[1024]{};
  printf("%s\n", argv[1]);
  strcpy(dev_name, argv[1]);

  LinuxSlave slave{argv[1]};
  slave.SetAddress(address);
  std::size_t loops = 0;

  fflush(stdout);
  while (true) {
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
