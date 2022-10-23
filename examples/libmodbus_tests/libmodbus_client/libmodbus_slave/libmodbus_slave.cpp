#include <modbus.h>
#include <unistd.h>

#include <cassert>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>

#include "DemoHeader.h"

int main(int argc, char *argv[]) {
  modbus_t *ctx = nullptr;
  if (argc < 2) {
    printf("Requires device port as argument, exiting\n");
    return 1;
  }
  assert(argc >= 2);
  char dev_name[1024]{};
  printf("%s\n", argv[1]);
  strcpy(dev_name, argv[1]);

  ConnectRtu(&ctx, dev_name);
  //  Prepare a Modbus mapping with 30 holding registers
  //  (plus no output coil, one input coil and two input registers)
  //  This will also automatically set the value of each register to 0
  modbus_mapping_t *mapping = modbus_mapping_new(256, 256, 256, 256);
  mapping->tab_registers[12] = 623;

  // Set the Modbus address of the remote slave
  modbus_set_slave(ctx, 246);

  if (modbus_connect(ctx) == -1) {
    fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
    modbus_free(ctx);
    return -1;
  }
  fprintf(stdout, "Connection Successful\n");

  uint8_t req[MODBUS_RTU_MAX_ADU_LENGTH]{};  // request buffer
  int len = 0;                               // length of the request/response

  printf("\n");

  std::size_t loops = 0;
  while (1) {
    loops++;

    len = modbus_receive(ctx, req);
    if (len == -1) break;

    len = modbus_reply(ctx, req, len, mapping);
    for (std::size_t i = 0; i < len; i++) {
      std::cout << static_cast<int>(req[i]) << ", ";
    }
    std::cout << std::endl;
    if (len == -1) break;

#if 0
  if (loops%16 == 0) {
      printf("%d \n[ ", mapping->nb_registers);
        for (std::size_t i = 0; i < 256; i++) {
          printf(" 0x%x", static_cast<uint16_t>(mapping->tab_registers[i]));
        }
      printf(" ]\n ");
    }
#endif
  }
  printf("Exit the loop: %s\n", modbus_strerror(errno));

  modbus_mapping_free(mapping);
  modbus_close(ctx);
  modbus_free(ctx);
}
