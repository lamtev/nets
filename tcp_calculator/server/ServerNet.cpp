//
// Created by anton.lamtev on 30.09.2018.
//

#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <thread>
#include <unistd.h>

#include <nets_lib/receivenbytes.h>
#include <protocol/CalculatorProtocol.h>
#include <protocol/Message.h>
#include <protocol/Operation.h>

#include "ServerNet.h"
#include "ServerNetError.h"
#include "ClientSession.h"
#include "ServerNetDelegate.h"


ServerNet::ServerNet(uint16_t port) : port(port), clients(), clientsMutex(), idCounter(0) {
}

void ServerNet::setDelegate(ServerNetDelegate *delegate) {
    this->delegate = delegate;
}

void ServerNet::start() {
    listeningSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listeningSocket < 0) {
        if (delegate != nullptr) {
            delegate->netDidFailWithError(this, ServerNetError::SOCKET_CREATE_ERROR);
        }
        return;
    }

    sockaddr_in localAddress{};
    localAddress.sin_family = AF_INET;
    localAddress.sin_port = htons(port);
    localAddress.sin_addr.s_addr = htonl(INADDR_ANY);

    int enable = 1;
    setsockopt(listeningSocket, SOL_SOCKET, SO_REUSEADDR, (void *) &enable, sizeof(int));

#ifdef __APPLE__
    setsockopt(listeningSocket, SOL_SOCKET, SO_NOSIGPIPE, (void *) &enable, sizeof(int));
#endif

    int error = bind(listeningSocket, (sockaddr *) &localAddress, sizeof(localAddress));
    if (error != 0) {
        if (delegate != nullptr) {
            delegate->netDidFailWithError(this, ServerNetError::SOCKET_BIND_ERROR);
        }
        return;
    }

    error = listen(listeningSocket, 5);
    if (error != 0) {
        if (delegate != nullptr) {
            delegate->netDidFailWithError(this, ServerNetError::SOCKET_LISTEN_ERROR);
        }
        return;
    }

    auto acceptThread = std::thread([this]() {
        while (true) {
            int acceptedSocket = accept(listeningSocket, nullptr, nullptr);
            if (acceptedSocket == -1) {
                if (delegate != nullptr) {
                    delegate->netDidFailWithError(this, ServerNetError::SOCKET_ACCEPT_ERROR);
                    break;
                }
            }

            auto clientThread = new std::thread([acceptedSocket, this]() {
                while (true) {
                    uint8_t bytesToBeReceived;
                    ssize_t bytesReceived = recv(acceptedSocket, &bytesToBeReceived, 1, 0);

                    if (bytesReceived == -1 || bytesReceived == 0) {
                        if (delegate != nullptr) {
                            delegate->netDidFailWithError(this, ServerNetError::SOCKET_RECEIVE_ERROR);
                        }
                        break;
                    }

                    auto bytes = new uint8_t[bytesToBeReceived + 1];
                    bytes[0] = bytesToBeReceived;

                    bytesReceived = receiveNBytes(acceptedSocket, bytesToBeReceived, bytes);
                    if (bytesReceived == -1 || bytesReceived == 0) {
                        if (delegate != nullptr) {
                            delegate->netDidFailWithError(this, ServerNetError::SOCKET_RECEIVE_ERROR);
                        }
                        break;
                    }

                    auto message = CalculatorProtocol::decode(bytes);

                    //TODO: handle message

                    switch (message->type()) {
                    case MessageType::MATH_RESPONSE: {
                        auto operation = Operation::of(bytes);
                        //TODO: handle operation

                        delete operation;
                        break;
                        //TODO: handle other message types
                    }
                    default:
                        break;
                    }

                    delete message;
                    delete[] bytes;

#ifdef __APPLE__
                    ssize_t bytesSent = send(acceptedSocket, nullptr, 0, 0);
#else
                    ssize_t bytesSent = send(acceptedSocket, nullptr, 0, MSG_NOSIGNAL);
#endif
                    if (bytesSent == -1) {
                        if (delegate != nullptr) {
                            delegate->netDidFailWithError(this, ServerNetError::SOCKET_SEND_ERROR);
                        }
                    }
                }


            });

            std::unique_lock<std::shared_mutex> lock(clientsMutex);
            clients.emplace_back(nextId(), acceptedSocket, clientThread);
        }

        std::unique_lock<std::shared_mutex> lock(clientsMutex);
        for (auto &client : clients) {
            close(client.socket());
            client.thread()->join();
            delete client.thread();
        }
        clients.clear();

    });

    acceptThread.join();
}

void ServerNet::stop() {
    closeSocket(listeningSocket);
}

uint64_t ServerNet::nextId() noexcept {
    return idCounter++;
}

void ServerNet::closeSocket(int socket) {
    shutdown(socket, SHUT_RDWR);
    close(socket);
}

void ServerNet::ioWantsToKillClientWithId(ServerIO *io, uint64_t id) {
    std::unique_lock<std::shared_mutex> lock(clientsMutex);
    auto toBeRemoved = std::find_if(clients.cbegin(), clients.cend(), [id](const ClientSession &client) -> bool {
        return client.id() == id;
    });

    if (toBeRemoved.base() != nullptr) {
        closeSocket(toBeRemoved->socket());
        toBeRemoved->thread()->join();
    } else if (delegate != nullptr) {
        delegate->netDidFailWithError(this, ServerNetError::KILL_CLIENT_ERROR);
    }

    clients.erase(toBeRemoved);
}

std::vector<ClientSession> ServerNet::ioWantsToListClients(ServerIO *io) {
    std::shared_lock<std::shared_mutex> lock(clientsMutex);
    auto clientsCopy = clients;
    lock.unlock();
    return clientsCopy;
}

void ServerNet::ioWantsToExit(ServerIO *io) {
    closeSocket(listeningSocket);
}