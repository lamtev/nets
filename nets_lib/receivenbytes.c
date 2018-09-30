//
// Created by anton.lamtev on 14.09.2018.
//

#include <sys/socket.h>

long receiveNBytes(int socket, size_t n, void *bytes) {
    size_t received = 0;
    while (received < n && received >= 0) {
        ssize_t recvd = recv(socket, &bytes[received], n - received, 0);
        if (recvd == -1) {
            return -1;
        } else if (recvd == 0) {
            // If TCP socket
            // the peer has closed its half side of the connection.
            return 0;
        }
        received += recvd;
    }

    return n;
}
