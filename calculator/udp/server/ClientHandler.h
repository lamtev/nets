//
// Created by anton.lamtev on 09/11/2018.
//

#ifndef NETS_CLIENTHANDLER_H
#define NETS_CLIENTHANDLER_H

#include <nets_lib/SockAddr.h>

#include <functional>
#include <vector>
#include <thread>
#include <atomic>

class Message;


class ClientHandler {
public:
    explicit ClientHandler(const SockAddr &addr);

    void submit(
            uint8_t *data,
            size_t size,
            const std::function<void(uint64_t ackNumber)> &ackCallback,
            const std::function<void(uint8_t *data, size_t size, uint64_t msgNumber)> &responseCallback);

    std::atomic<bool> ackReceived;

private:
    std::vector<std::thread *> threads;

private:
    Message *handleRequest(Message *request, int socket);

};

#endif //NETS_CLIENTHANDLER_H
