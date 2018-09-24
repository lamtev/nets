//
// Created by anton.lamtev on 21.09.2018.
//

#ifndef NETS_LIB_RECEIVE_UP_TO_DELIMITER_H
#define NETS_LIB_RECEIVE_UP_TO_DELIMITER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>

size_t receiveUpToDelimiter(int socket, char delimiter, char *bytes);

#ifdef __cplusplus
}
#endif

#endif //NETS_LIB_RECEIVE_UP_TO_DELIMITER_H
