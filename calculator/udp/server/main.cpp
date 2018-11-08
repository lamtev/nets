//
// Created by anton.lamtev on 30/10/2018.
//

#include <iostream>
#include <unordered_map>
#include <chrono>

#include <netinet/in.h>

#include <calculator/protocol/Message.h>
#include <calculator/protocol/NumberedMessage.h>
#include <calculator/protocol/Operation.h>
#include <calculator/protocol/MathResponse.h>
#include <calculator/protocol/BitsUtils.h>
#include <nets_lib/send.h>
#include <nets_lib/SockAddr.h>

#include "ClientHandler.h"

std::atomic<uint64_t> cnt = 0;
std::unordered_map<SockAddr, ClientHandler *> clientHandlers;

int main(int argc, char **argv) {
    sockaddr_in local{};
    local.sin_port = htons(1234);
    local.sin_addr.s_addr = htonl(INADDR_ANY);
    local.sin_family = AF_INET;

    int socket = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (socket < 0) {
        std::cerr << "Socket error: " << strerror(errno) << std::endl;
        return 1;
    }

    if (bind(socket, (sockaddr *) &local, sizeof(local)) != 0) {
        std::cerr << "Unable to bind: " << strerror(errno) << std::endl;
        return 1;
    }

    sockaddr_in peer{};
    unsigned int peerlen;
    uint8_t buf[65507];
    while (true) {
        peerlen = sizeof(peer);
        ssize_t received = recvfrom(socket, buf, sizeof(buf), 0, (sockaddr *) &peer, &peerlen);
        if (received < 0) {
            std::cerr << "Unable to recvfrom: " << strerror(errno) << std::endl;
            return 1;
        } else if (received == 0) {
            std::cout << "0 bytes received" << std::endl;
        }

        auto addr = SockAddr(*((const sockaddr *) &peer), peerlen);
        auto handler = (clientHandlers[addr] = new ClientHandler(addr));
        auto data = new uint8_t[received];
        auto size = (size_t) received;
        std::memcpy(data, buf, size);

        handler->submit(data, size, /*ackCallback:*/[socket, peer, peerlen](uint64_t ackNumber) {
            auto ack = new NumberedMessage(ackNumber, new Message(MessageType::ACK, 0, nullptr));
            auto ackBytes = ack->toBytes();
            if (sendto(socket, ackBytes, ack->size(), 0, (sockaddr *) &peer, peerlen) < 0) {
                std::cerr << "Unable to sendto: " << strerror(errno) << std::endl;
                return;
            }
            delete ack;
            delete[] ackBytes;

            std::cout << "Ack " << ackNumber << " sent" << std::endl;
        }, /*responseCallback:*/[socket, peer, peerlen, handler](const uint8_t *data, size_t size, uint64_t msgNumber) {
            while (!handler->ackReceived) {
                if (sendto(socket, data, size, 0, (sockaddr *) &peer, peerlen) < 0) {
                    std::cerr << "Unable to sendto: " << strerror(errno) << std::endl;
                    return;
                }

                std::cout << "Response sent" << std::endl;
                std::cout << "Waiting for ack " << msgNumber << " ..." << std::endl;

                auto st = std::chrono::high_resolution_clock::now();
                while (!handler->ackReceived && std::chrono::duration_cast<std::chrono::seconds>(
                        std::chrono::high_resolution_clock::now() - st).count() < 1);
            }
            handler->ackReceived = false;
        });

    }
    return 0;
}