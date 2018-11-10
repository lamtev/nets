//
// Created by anton.lamtev on 10/11/2018.
//

#include "UDPServerNet.h"

#include <iostream>

#include <calculator/server/commons/ServerIO.h>
#include <calculator/server/commons/ServerNetDelegate.h>


UDPServerNet::UDPServerNet(uint16_t port) :
        port(port),
        delegate(nullptr),
        clientHandlers(),
        clientHandlersMutex() {}

void UDPServerNet::setDelegate(ServerNetDelegate *delegate) {
    this->delegate = delegate;
}

void UDPServerNet::start() {
    sockaddr_in local{};
    local.sin_port = htons(port);
    local.sin_addr.s_addr = htonl(INADDR_ANY);
    local.sin_family = AF_INET;

    int socket = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (socket < 0) {
        std::cerr << "Socket error: " << strerror(errno) << std::endl;
    }

    if (bind(socket, (sockaddr *) &local, sizeof(local)) != 0) {
        std::cerr << "Unable to bind: " << strerror(errno) << std::endl;
    }

    sockaddr_in peer{};
    unsigned int peerlen;
    uint8_t buf[65507];
    while (true) {
        peerlen = sizeof(peer);
        ssize_t received = recvfrom(socket, buf, sizeof(buf), 0, (sockaddr *) &peer, &peerlen);
        if (received < 0) {
            std::cerr << "Unable to recvfrom: " << strerror(errno) << std::endl;
            break;
        } else if (received == 0) {
            std::cout << "0 bytes received" << std::endl;
        }

        auto addr = SockAddr(peer, peerlen);
        ClientHandler *handler = nullptr;
        {
            std::lock_guard<std::mutex> lock(clientHandlersMutex);
            if (clientHandlers.find(addr) == clientHandlers.end()) {
                handler = new ClientHandler(addr);
                clientHandlers[addr] = handler;
            } else {
                handler = clientHandlers[addr];
            }
        }

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
}

void UDPServerNet::stop() {

}

void UDPServerNet::ioWantsToKillClientWithId(ServerIO *io, uint64_t id) {

}

std::vector<Client> UDPServerNet::ioWantsToListClients(ServerIO *io) {
    std::vector<Client> c;
    return c;
}

void UDPServerNet::ioWantsToExit(ServerIO *io) {

}
