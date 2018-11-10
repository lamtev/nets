//
// Created by anton.lamtev on 10/11/2018.
//

#include "UDPServerNet.h"

#include <arpa/inet.h>
#include <unistd.h>

#include <iostream>
#include <chrono>
#include <algorithm>

#include <calculator/server/commons/ServerIO.h>
#include <calculator/server/commons/ServerNetDelegate.h>

#include <calculator/protocol/Message.h>


UDPServerNet::UDPServerNet(uint16_t port) :
        port(port),
        socket(0),
        delegate(nullptr),
        clientHandlers(),
        clientHandlersMutex(),
        clientCounter(0),
        joinThreads() {}

void UDPServerNet::setDelegate(ServerNetDelegate *delegate) {
    this->delegate = delegate;
}

void UDPServerNet::start() {
    sockaddr_in local{};
    local.sin_port = htons(port);
    local.sin_addr.s_addr = htonl(INADDR_ANY);
    local.sin_family = AF_INET;

    socket = ::socket(AF_INET, SOCK_DGRAM, 0);
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
                handler = new ClientHandler(addr, clientCounter++);
                clientHandlers[addr] = handler;
            } else {
                handler = clientHandlers[addr];
            }
        }

        auto data = new uint8_t[received];
        auto size = (size_t) received;
        std::memcpy(data, buf, size);

        handler->submit(data, size, [this, peer, peerlen, handler](uint8_t *ackBytes, size_t ackSize, uint64_t ackNumber) {
            if (sendto(socket, ackBytes, ackSize, 0, (sockaddr *) &peer, peerlen) < 0) {
                std::cerr << "Unable to sendto: " << strerror(errno) << std::endl;
                return;
            }
            std::cout << "[Client id=" << handler->id << "]\t" << "Ack " << ackNumber << " sent"
                      << std::endl;
        }, [this, peer, peerlen, handler](uint8_t *responseBytes, size_t responseSize, uint64_t responseNumber) {
            int attemptsToReceiveAck = 0;
            while (!handler->ackReceived && attemptsToReceiveAck++ < 10) {
                if (sendto(socket, responseBytes, responseSize, 0, (sockaddr *) &peer, peerlen) < 0) {
                    std::cerr << "Unable to sendto: " << strerror(errno) << std::endl;
                    return;
                }

                std::cout << "[Client id=" << handler->id << "]\t" << "Response " << responseNumber << " sent"
                          << std::endl;
                std::cout << "[Client id=" << handler->id << "]\t" << "Waiting for ack " << responseNumber
                          << " ..." << std::endl;

                using namespace std::chrono;
                auto timestamp = high_resolution_clock::now();
                while (!handler->ackReceived && duration_cast<seconds>(high_resolution_clock::now() - timestamp).count() < 1);
            }
            handler->ackReceived = false;
        });
    }

    stop();
}

void UDPServerNet::stop() {
    shutdown(socket, SHUT_RDWR);
    close(socket);

    std::lock_guard<std::mutex> lock(clientHandlersMutex);
    for (auto client :clientHandlers) {
        delete client.second;//joins all client threads
    }
    clientHandlers.clear();
}

void UDPServerNet::ioWantsToKillClientWithId(ServerIO *io, uint64_t id) {}

std::vector<Client> UDPServerNet::ioWantsToListClients(ServerIO *io) {
    std::vector<Client> clients;
    std::lock_guard<std::mutex> lock(clientHandlersMutex);
    for (auto client : clientHandlers) {
        sockaddr_in addr = *((sockaddr_in *) &client.first.addr);
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(addr.sin_addr), ip, INET_ADDRSTRLEN);
        clients.emplace_back(client.second->id, ip, addr.sin_port);
    }

    return clients;
}

void UDPServerNet::ioWantsToExit(ServerIO *io) {
    stop();
}
