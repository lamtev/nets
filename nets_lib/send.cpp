//
// Created by anton.lamtev on 09.10.2018.
//

#include <sys/socket.h>


namespace netslib {

ssize_t send(int socket, void *bytes, size_t size) {
#ifdef __APPLE__
    return ::send(socket, bytes, size, 0);
#else
    return ::send(socket, bytes, result.size(), MSG_NOSIGNAL);
#endif
}

}

