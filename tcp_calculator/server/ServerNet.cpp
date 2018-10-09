//
// Created by anton.lamtev on 30.09.2018.
//

#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <thread>
#include <unistd.h>
#include <cmath>
#include <chrono>

#include <nets_lib/receivenbytes.h>
#include <nets_lib/send.h>
#include <protocol/Message.h>
#include <protocol/Operation.h>
#include <protocol/BitsUtils.h>
#include <protocol/MathResponse.h>

#include "ServerNet.h"
#include "ServerNetError.h"
#include "ClientSession.h"
#include "ServerNetDelegate.h"
#include "MathUtils.h"

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

            u_int64_t clientId = nextId();
            auto clientThread = new std::thread([acceptedSocket, this, clientId]() {
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

                    auto response = handleRequest(request, acceptedSocket);


                    auto bytesToBeSent = response->toBytes();
                    delete[] response->data();
                    delete request;
                    delete[] bytes;

                    ssize_t bytesSent = netslib::send(acceptedSocket, bytesToBeSent, response->size());
                    
                    delete response;
                    delete[] bytesToBeSent;

                    if (bytesSent == -1) {
                        if (delegate != nullptr) {
                            delegate->netDidFailWithError(this, ServerNetError::SOCKET_SEND_ERROR);
                        }
                    }
                }
//                ioWantsToKillClientWithId(nullptr, clientId);
            });

            std::unique_lock<std::shared_mutex> lock(clientsMutex);
            clients.emplace_back(clientId, acceptedSocket, clientThread);
        }

        std::unique_lock<std::shared_mutex> lock(clientsMutex);
        for (auto &client : clients) {
            close(client.socket());
            client.thread()->join();
            delete client.thread();
        }
        clients.clear();

        for (auto th : hardOperationThreadPool) {
            th->join();
            delete th;
        }
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

Message *ServerNet::handleRequest(Message *request, int socket) {
    MessageType responseType;
    uint8_t dataSize;
    uint8_t *data;
    switch (request->type()) {
    case MessageType::MATH_REQUEST: {
        auto operation = Operation::of(request->data());
        responseType = MessageType::MATH_RESPONSE;
        int64_t res;
        MathResponseType mathResponseType;
        switch (operation->type()) {
        case OperationType::ADDITION:
            res = operation->operand1() + operation->operand2();
            mathResponseType = MathResponseType::FAST_OPERATION_RESULT;
            break;
        case OperationType::SUBTRACTION:
            res = operation->operand1() - operation->operand2();
            mathResponseType = MathResponseType::FAST_OPERATION_RESULT;
            break;
        case OperationType::MULTIPLICATION:
            res = operation->operand1() * operation->operand2();
            mathResponseType = MathResponseType::FAST_OPERATION_RESULT;
            break;
        case OperationType::DIVISION: {
            auto op2 = operation->operand2();
            if (op2 == 0) {
                res = 0;
                mathResponseType = MathResponseType::BAD_OPERATION;
            } else {
                res = operation->operand1() / op2;
                mathResponseType = MathResponseType::FAST_OPERATION_RESULT;
            }
            break;
        }
        case OperationType::FACTORIAL:
        case OperationType::SQUARE_ROOT:
            res = 0;
            mathResponseType = MathResponseType::HARD_OPERATION_SUBMITTED;
            submitHardOperation(*operation, socket);
            break;
        default:
            res = 0;
            mathResponseType = MathResponseType::BAD_OPERATION;
            break;
        }
        delete operation;
        MathResponse mathResponse = MathResponse(mathResponseType, res);
        dataSize = mathResponse.size();
        data = mathResponse.toBytes();
        break;
    }
    case MessageType::CONTROL_REQUEST: {
        if (*request->data() == 0x00) {
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

void ServerNet::submitHardOperation(Operation operation, int socket) {
    //TODO: GNU Multi-Precision Library
    std::lock_guard<std::mutex> lock(hardOperationThreadPoolMutex);
    auto hardOperationThread = new std::thread([this, operation, socket]() {
        std::this_thread::sleep_for(std::chrono::seconds(1));
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
    hardOperationThreadPool.push_back(hardOperationThread);
}

void ServerNet::ioWantsToKillClientWithId(ServerIO *io, uint64_t id) {
    std::unique_lock<std::shared_mutex> lock(clientsMutex);
    auto toBeRemoved = std::find_if(clients.cbegin(), clients.cend(), [id](const ClientSession &client) -> bool {
        return client.id() == id;
    });

    if (toBeRemoved.base() != nullptr) {
        closeSocket(toBeRemoved->socket());
        toBeRemoved->thread()->join();
        clients.erase(toBeRemoved);
    } else if (delegate != nullptr) {
        delegate->netDidFailWithError(this, ServerNetError::KILL_CLIENT_ERROR);
    }
}

std::vector<ClientSession> ServerNet::ioWantsToListClients(ServerIO *io) {
    std::shared_lock<std::shared_mutex> lock(clientsMutex);
    return clients;
}

void ServerNet::ioWantsToExit(ServerIO *io) {
    closeSocket(listeningSocket);
}
