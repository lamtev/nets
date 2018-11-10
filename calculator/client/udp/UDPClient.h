//
// Created by anton.lamtev on 08/11/2018.
//

#ifndef NETS_UDPCLIENT_H
#define NETS_UDPCLIENT_H

#include <atomic>


class UDPClient {
public:
    UDPClient(const char *addr, int port);

    void start();

    void stop();

private:
    const char *addr;
    int port;
    int socket;

    std::atomic<bool> waitingForAck;
    std::atomic<bool> ackReceived;
    std::atomic<bool> timeout;
    std::atomic<bool> responseReceived;

    std::atomic<uint64_t> messageCounter;
};

#endif //NETS_UDPCLIENT_H
