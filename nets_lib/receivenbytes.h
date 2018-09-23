//
// Created by anton.lamtev on 14.09.2018.
//

#ifndef NETS_LIB_RECEIVE_N_BYTES_H
#define NETS_LIB_RECEIVE_N_BYTES_H

#include <sys/types.h>

size_t receiveNBytes(int socket, size_t n, char *bytes);

#endif //NETS_LIB_RECEIVE_N_BYTES_H
