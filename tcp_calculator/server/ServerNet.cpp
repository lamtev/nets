//
// Created by anton.lamtev on 30.09.2018.
//

#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <thread>

#include <nets_lib/receivenbytes.h>
#include <protocol/CalculatorProtocol.h>
#include <protocol/Message.h>
#include <protocol/Operation.h>

#include "ServerNet.h"

ServerNet::ServerNet(uint16_t port) : port(port) {
}

void ServerNet::setDelegate(ServerNetDelegate *delegate) {
    this->delegate = delegate;
}

void ServerNet::start() {
    int listeningSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listeningSocket < 0) {
        if (delegate != nullptr) {
            delegate->netDidFailWithError(this, ServerNetError::SOCKET_CREATE_ERROR);
        }
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
    }

    error = listen(listeningSocket, 5);
    if (error != 0) {
        if (delegate != nullptr) {
            delegate->netDidFailWithError(this, ServerNetError::SOCKET_LISTEN_ERROR);
        }
    }
    //TODO: join accept thread
    //TODO: join client threads
    auto acceptThread = std::thread([listeningSocket, this]() {
        while (true) {
            int acceptedSocket = accept(listeningSocket, nullptr, nullptr);
            if (acceptedSocket == -1) {
                if (delegate != nullptr) {
                    delegate->netDidFailWithError(this, ServerNetError::SOCKET_ACCEPT_ERROR);
                    break;
                }
            }

            auto clientThread = std::thread([acceptedSocket, this]() {
                while (true) {
                    u_int8_t bytesToBeReceived;
                    ssize_t bytesReceived = recv(acceptedSocket, &bytesToBeReceived, 1, 0);

                    if (bytesReceived == -1 || bytesReceived == 0) {
                        if (delegate != nullptr) {
                            delegate->netDidFailWithError(this, ServerNetError::RECEIVE_ERROR);
                        }
                        break;
                    }

                    auto bytes = new uint8_t[bytesToBeReceived + 1];
                    bytes[0] = bytesToBeReceived;

                    bytesReceived = receiveNBytes(acceptedSocket, bytesToBeReceived, bytes);
                    if (bytesReceived == -1 || bytesReceived == 0) {
                        if (delegate != nullptr) {
                            delegate->netDidFailWithError(this, ServerNetError::RECEIVE_ERROR);
                        }
                        break;
                    }

                    auto message = CalculatorProtocol::decode(bytes);

                    //TODO: handle message

                    switch (message->type()) {
                    case MessageType::MATH_RESPONSE: {
                        auto operation = reinterpret_cast<Operation *>(message->data());
                        //TODO: handle operation
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
                            delegate->netDidFailWithError(this, ServerNetError::SEND_ERROR);
                        }
                    }
                }
            });
        }
    });
}

void ServerNet::stop() {

}
