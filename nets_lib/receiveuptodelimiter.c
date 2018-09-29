//
// Created by anton.lamtev on 21.09.2018.
//

#include <sys/socket.h>
#include "receiveuptodelimiter.h"


size_t receiveUpToDelimiter(int socket, char delimiter, void *bytes) {
  size_t received = 0;
  while (((char *) bytes)[received] != delimiter) {
    received += recv(socket, &bytes[received], 1, 0);
  }

  return received;
}
