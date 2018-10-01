//
// Created by anton.lamtev on 01.10.2018.
//

#ifndef NETS_CLIENTSESSION_H
#define NETS_CLIENTSESSION_H

#include <thread>

class ClientSession {
 private:
    uint64_t _id;
    int _socket;
    std::thread *_thread;

 public:
    ClientSession(uint64_t id, int socket, std::thread *thread) noexcept : _id(id), _socket(socket), _thread(thread) {
    }

    uint64_t id() const noexcept {
        return _id;
    }

    int socket() const noexcept {
        return _socket;
    }

    std::thread *thread() const noexcept {
        return _thread;
    }

};

#endif //NETS_CLIENTSESSION_H
