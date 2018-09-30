//
// Created by anton.lamtev on 30.09.2018.
//

#include <string>
#include <iostream>

#include "ServerIO.h"


ServerIO::ServerIO() : isRunning(true) {
}

void ServerIO::start() {
    while (isRunning) {
        std::string instruction;
        std::getline(std::cin, instruction);

        if ("exit" == instruction) {
            break;
        } else if ("list" == instruction) {

        } else {
            auto idx = instruction.find_last_of("kill");
            if (idx != std::string::npos) {
                size_t clientIndex;
                try {
                    clientIndex = static_cast<size_t>(std::stoul(instruction.substr(idx + 1)));
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
    case ServerNetError::RECEIVE_ERROR:
        std::cerr << "Unable to receive: " << strerror(errno) << std::endl;
        break;
    case ServerNetError::SEND_ERROR:
        std::cerr << "Unable to send: " << strerror(errno) << std::endl;
        return;
    default:
        return;
    }

    if (net != nullptr) {
        net->stop();
    }
}
