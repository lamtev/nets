//
// Created by anton.lamtev on 10/11/2018.
//

#ifndef NETS_CLIENT_H
#define NETS_CLIENT_H

#include <string>
#include <cinttypes>

struct Client {
    uint64_t id;
    std::string ip;
    uint16_t port;

    Client(uint64_t id, const std::string &ip, uint16_t port) {
        this->id = id;
        this->ip = ip;
        this->port = port;
    }
};

#endif //NETS_CLIENT_H
