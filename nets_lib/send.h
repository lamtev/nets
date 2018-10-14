//
// Created by anton.lamtev on 09.10.2018.
//

#ifndef NETS_SEND_H
#define NETS_SEND_H

#include <sys/types.h>


namespace netslib {

ssize_t send(int socket, void *bytes, size_t size);

}

#endif //NETS_SEND_H
