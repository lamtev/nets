//
// Created by anton.lamtev on 30/10/2018.
//

#include "Balancer.h"

#include <thread>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>


void Balancer::start() {
    sockaddr_in local{};
    local.sin_port = htons(1234);
    local.sin_addr.s_addr = htonl(INADDR_ANY);
    local.sin_family = AF_INET;

    int socket = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (socket < 0) {
        std::cerr << "Socket error: " << strerror(errno) << std::endl;
        return;
    }

    if (bind(socket, (sockaddr *) &local, sizeof(local)) != 0) {
        std::cerr << "Unable to bind: " << strerror(errno) << std::endl;
        return;
    }

    std::thread receive([] {

    });

    std::thread send([] {

    });
}

void Balancer::stop() {

}
