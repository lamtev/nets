#include <utility>

//
// Created by anton.lamtev on 01.10.2018.
//

#ifndef NETS_CLIENTSESSION_H
#define NETS_CLIENTSESSION_H

#include <thread>


struct ClientSession {
    uint64_t id;
    int socket;

    ClientSession(uint64_t id, int socket) noexcept :
            id(id),
            socket(socket) {}

};

#endif //NETS_CLIENTSESSION_H
