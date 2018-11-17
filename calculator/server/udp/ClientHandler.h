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
#include <mutex>

class Message;
class NumberedMessage;
class Operation;


using Callback = std::function<void(uint8_t *data, size_t size, uint64_t number)>;

class ClientHandler {
public:
    explicit ClientHandler(const SockAddr &addr, uint64_t id);

    ~ClientHandler();

    void submit(
            uint8_t *data,
            size_t size,
            const Callback &ackCallback,
            const Callback &responseCallback
    );

    std::atomic<bool> ackReceived;

    const uint64_t id;

private:
    SockAddr sockAddr;
    std::atomic<uint64_t> expectedRequestNumber;
    std::atomic<uint64_t> responseNumber;
    std::vector<std::thread *> threads;
    std::mutex threadsMutex;

private:
    Message *handleRequest(NumberedMessage *request, Operation **hardOperation);

    void handleHardOperation(
            Operation *hardOperation,
            const Callback &responseCallback
    );

    void buildAckWithNumber(uint64_t number, uint8_t **ackData, size_t *ackSize);

};

#endif //NETS_CLIENTHANDLER_H
