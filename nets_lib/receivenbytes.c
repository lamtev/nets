//
// Created by anton.lamtev on 14.09.2018.
//

#include <sys/socket.h>
#include <stdlib.h>
#include <memory.h>

size_t receiveNBytes(int socket, size_t n, char *bytes) {
  size_t received = 0;
  while (received < n && received >= 0) {
    received += recv(socket, &bytes[received], n - received, 0);
  }

  return received < 0 ? -1 : n;
}
