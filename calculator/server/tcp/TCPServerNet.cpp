//
// Created by anton.lamtev on 30.09.2018.
//

#include "TCPServerNet.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <iostream>
#include <cmath>
#include <algorithm>

#include <nets_lib/receivenbytes.h>
#include <nets_lib/send.h>

#include <calculator/protocol/Message.h>
#include <calculator/protocol/Operation.h>
#include <calculator/protocol/MathResponse.h>
#include <calculator/protocol/MathUtils.h>
#include <calculator/protocol/BitsUtils.h>

#include <calculator/server/commons/ServerNetError.h>
#include <calculator/server/commons/ServerNetDelegate.h>


TCPServerNet::TCPServerNet(uint16_t port) :
        port(port),
        listeningSocket(),
        delegate(nullptr),
        clients(),
        clientsMutex(),
        idCounter(0),
        clientThreadsMutex(),
        clientThreads() {}

void TCPServerNet::setDelegate(ServerNetDelegate *delegate) {
    this->delegate = delegate;
}

void TCPServerNet::start() {
    listeningSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
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

    while (true) {
        int acceptedSocket = accept(listeningSocket, nullptr, nullptr);
        if (acceptedSocket == -1) {
            if (delegate != nullptr) {
                delegate->netDidFailWithError(this, ServerNetError::SOCKET_ACCEPT_ERROR);
                break;
            }
        }

        u_int64_t clientId = nextId();
        std::thread clientThread([acceptedSocket, this, clientId] {
            while (true) {
                uint8_t bytesInData;
                ssize_t bytesReceived = recv(acceptedSocket, &bytesInData, 1, 0);
                if (bytesReceived == -1 || bytesReceived == 0) {
                    if (delegate != nullptr) {
                        delegate->netDidFailWithError(this, ServerNetError::SOCKET_RECEIVE_ERROR);
                    }
                    break;
                }
                uint8_t bytesToBeReceived = bytesInData + uint8_t(1);

                auto bytes = new uint8_t[bytesToBeReceived];
                bytes[0] = bytesInData;

                bytesReceived = receiveNBytes(acceptedSocket, bytesInData, &bytes[1]);
                if (bytesReceived == -1 || bytesReceived == 0) {
                    if (delegate != nullptr) {
                        delegate->netDidFailWithError(this, ServerNetError::SOCKET_RECEIVE_ERROR);
                    }
                    break;
                }

                auto request = Message::of(bytes);

                auto response = handleRequest(request, acceptedSocket, clientId);

                auto bytesToBeSent = response->toBytes();

                delete[] response->data();
                delete request;
                delete[] bytes;

                ssize_t bytesSent = netslib::send(acceptedSocket, bytesToBeSent, response->size());

                delete[] bytesToBeSent;
                delete response;

                if (bytesSent == -1) {
                    if (delegate != nullptr) {
                        delegate->netDidFailWithError(this, ServerNetError::SOCKET_SEND_ERROR);
                    }
                }
            }

            std::unique_lock<std::shared_mutex> lock(clientsMutex);
            clients.erase(clientId);
            closeSocket(acceptedSocket);
        });
        {
            std::lock_guard<std::mutex> lock(clientThreadsMutex);
            clientThreads.push_back(std::move(clientThread));
        }

        {
            std::unique_lock<std::shared_mutex> lock(clientsMutex);
            clients[clientId] = acceptedSocket;
        }
    }

    {
        std::unique_lock<std::shared_mutex> lock(clientsMutex);
        for (auto &client : clients) {
            closeSocket(client.second);
        }
        clients.clear();
    }

    {
        std::lock_guard<std::mutex> lock(clientThreadsMutex);
        for (std::thread &thread : clientThreads) {
            thread.join();
        }
        clientThreads.clear();
    }
}

void TCPServerNet::stop() {
    closeSocket(listeningSocket);
}

uint64_t TCPServerNet::nextId() noexcept {
    return idCounter++;
}

void TCPServerNet::closeSocket(int socket) {
    shutdown(socket, SHUT_RDWR);
    close(socket);
}

Message *TCPServerNet::handleRequest(Message *request, int socket, uint64_t clientId) {
    MessageType responseType;
    uint8_t dataSize;
    uint8_t *data;
    switch (request->type()) {
        case MessageType::MATH_REQUEST: {
            auto operation = Operation::of(request->data());
            responseType = MessageType::MATH_RESPONSE;
            int64_t result;
            MathResponseType mathResponseType;
            switch (operation->type()) {
                case OperationType::ADDITION:
                    result = operation->operand1() + operation->operand2();
                    mathResponseType = MathResponseType::FAST_OPERATION_RESULT;
                    break;
                case OperationType::SUBTRACTION:
                    result = operation->operand1() - operation->operand2();
                    mathResponseType = MathResponseType::FAST_OPERATION_RESULT;
                    break;
                case OperationType::MULTIPLICATION:
                    result = operation->operand1() * operation->operand2();
                    mathResponseType = MathResponseType::FAST_OPERATION_RESULT;
                    break;
                case OperationType::DIVISION: {
                    auto operand2 = operation->operand2();
                    if (operand2 == 0) {
                        result = 0;
                        mathResponseType = MathResponseType::BAD_OPERATION;
                    } else {
                        result = operation->operand1() / operand2;
                        mathResponseType = MathResponseType::FAST_OPERATION_RESULT;
                    }
                    break;
                }
                case OperationType::FACTORIAL:
                case OperationType::SQUARE_ROOT:
                    result = 0;
                    mathResponseType = MathResponseType::HARD_OPERATION_SUBMITTED;
                    submitHardOperation(*operation, socket);
                    break;
                default:
                    result = 0;
                    mathResponseType = MathResponseType::BAD_OPERATION;
                    break;
            }
            delete operation;
            MathResponse mathResponse = MathResponse(mathResponseType, result);
            dataSize = mathResponse.size();
            data = mathResponse.toBytes();
            break;
        }
        case MessageType::CONTROL_REQUEST: {
            if (*request->data() == 0x00) { //0x00 for CONTROL_REQUEST means "kill me". It would be better if i will add an enum
                std::shared_lock<std::shared_mutex> lock(clientsMutex);

                int clientSocket = clients[clientId];
                bool clientWasNotRemoved = clientSocket == socket;
                if (clientWasNotRemoved) {
                    closeSocket(socket);
                    clients.erase(clientId);
                } else if (delegate != nullptr) {
                    delegate->netDidFailWithError(this, ServerNetError::KILL_CLIENT_ERROR);
                }
                lock.unlock();
                data = new uint8_t[1];
                *data = 0x00;
                dataSize = 1;
                responseType = MessageType::CONTROL_RESPONSE;
            } else {
                return nullptr;
            }
            break;
        }
        default:
            return nullptr;
    }

    return new Message(responseType, dataSize, data);
}

void TCPServerNet::submitHardOperation(const Operation &operation, int socket) {
    //TODO: GNU Multi-Precision Library
    std::lock_guard<std::mutex> lock(clientThreadsMutex);
    clientThreads.emplace_back([this, operation, socket]() {
        std::this_thread::sleep_for(std::chrono::seconds(20)); //make operation really hard :)
        int64_t res;
        switch (operation.type()) {
            case OperationType::FACTORIAL:
                res = factorial(static_cast<uint64_t>(operation.operand1()));
                break;
            case OperationType::SQUARE_ROOT:
                res = static_cast<int64_t>(sqrt(operation.operand1()));
                break;
            default:
                return;
        }
        auto *data = new uint8_t[8];
        int64AsBytes(res, data);
        auto result = Message(MessageType::SERVER_INITIATED_REQUEST, 8, data);
        auto bytes = result.toBytes();

        ssize_t bytesSent = netslib::send(socket, bytes, result.size());

        if (bytesSent == -1) {
            if (delegate != nullptr) {
                delegate->netDidFailWithError(this, ServerNetError::SOCKET_SEND_ERROR);
            }
        }

        delete[] data;
        delete[] bytes;
    });
}

void TCPServerNet::ioWantsToKillClientWithId(ServerIO *io, uint64_t id) {
    std::unique_lock<std::shared_mutex> lock(clientsMutex);
    int socket = clients[id];

    if (socket != 0) {
        clients.erase(id);
        closeSocket(socket);
    } else if (delegate != nullptr) {
        delegate->netDidFailWithError(this, ServerNetError::KILL_CLIENT_ERROR);
    }
}

std::vector<Client> TCPServerNet::ioWantsToListClients(ServerIO *io) {
    std::shared_lock<std::shared_mutex> lock(clientsMutex);
    std::vector<Client> ioClients;
    for (const auto &client : clients) {
        sockaddr_in addr{};
        socklen_t size = sizeof(sockaddr_in);
        getpeername(client.second, (sockaddr *) &addr, &size);
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(addr.sin_addr), ip, INET_ADDRSTRLEN);
        ioClients.emplace_back(client.first, ip, addr.sin_port);
    }

    return ioClients;
}

void TCPServerNet::ioWantsToExit(ServerIO *io) {
    stop();
}
