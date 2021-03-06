//
// Created by anton.lamtev on 14.09.2018.
//

#ifndef NETS_LIB_RECEIVE_N_BYTES_H
#define NETS_LIB_RECEIVE_N_BYTES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>

ssize_t receiveNBytes(int socket, size_t n, void *bytes);

#ifdef __cplusplus
}
#endif

#endif //NETS_LIB_RECEIVE_N_BYTES_H
