//
// Created by anton.lamtev on 30.09.2018.
//

#include <string>
#include <iostream>
#include <vector>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "ServerIO.h"
#include "ClientSession.h"
#include "ServerIODelegate.h"


ServerIO::ServerIO() : isRunning(true) {
}

void ServerIO::start() {
    while (isRunning) {
        std::string instruction;
        std::getline(std::cin, instruction);

        if ("exit" == instruction) {
            if (delegate != nullptr) {
                delegate->ioWantsToExit(this);
            }
            break;
        } else if ("list" == instruction) {
            if (delegate != nullptr) {
                clients = delegate->ioWantsToListClients(this);
                int i = 0;
                for (auto &client : clients) {
                    sockaddr_in addr{};
                    getpeername(client.socket(), (sockaddr *) &addr, nullptr);
                    char ip[INET_ADDRSTRLEN];
                    inet_ntop(AF_INET, &(addr.sin_addr), ip, INET_ADDRSTRLEN);
                    std::cout << ++i << ".\t" << "id=" << client.id() << "\t" << ip << ":" << addr.sin_port
                              << std::endl;
                }
            }
        } else {
            auto idx = instruction.find_last_of("kill");
            if (idx != std::string::npos) {
                size_t clientIndex;
                try {
                    clientIndex = static_cast<size_t>(std::stoul(instruction.substr(idx + 1))) - 1;
                    if (delegate != nullptr) {
                        if (clients.empty()) {
                            clients = delegate->ioWantsToListClients(this);
                        }
                        if (clients.size() > clientIndex) {
                            delegate->ioWantsToKillClientWithId(this, clients[clientIndex].id());
                        } else {
                            std::cerr << "Client with specified index does not exist" << std::endl;
                        }
                    }
                } catch (const std::invalid_argument &ignored) {
                    continue;
                }
            }
        }
    }
}

void ServerIO::stop() {
    isRunning = false;
}

void ServerIO::setDelegate(ServerIODelegate *delegate) {
    this->delegate = delegate;
}

void ServerIO::netDidFailWithError(ServerNet *net, ServerNetError error) {
    switch (error) {
    case ServerNetError::SOCKET_CREATE_ERROR:
        std::cerr << "Socket error: " << strerror(errno) << std::endl;
        break;
    case ServerNetError::SOCKET_BIND_ERROR:
        std::cerr << "Unable to bind: " << strerror(errno) << std::endl;
        break;
    case ServerNetError::SOCKET_LISTEN_ERROR:
        std::cerr << "Unable to listen: " << strerror(errno) << std::endl;
        break;
    case ServerNetError::SOCKET_ACCEPT_ERROR:
        std::cerr << "Unable to accept: " << strerror(errno) << std::endl;
        break;
    case ServerNetError::SOCKET_RECEIVE_ERROR:
        std::cerr << "Unable to receive: " << strerror(errno) << std::endl;
        break;
    case ServerNetError::SOCKET_SEND_ERROR:
        std::cerr << "Unable to send: " << strerror(errno) << std::endl;
        break;
    case ServerNetError::KILL_CLIENT_ERROR:
        std::cerr << "Unable to kill client with specified index" << std::endl;
        break;
    default:
        return;
    }
}
