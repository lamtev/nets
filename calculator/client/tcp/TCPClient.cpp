//
// Created by anton.lamtev on 04.10.2018.
//

#include "TCPClient.h"

#include <iostream>
#include <vector>
#include <thread>
#include <cstring>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <nets_lib/receivenbytes.h>

#include <calculator/protocol/BitsUtils.h>
#include <calculator/protocol/MathResponse.h>
#include <calculator/protocol/Message.h>
#include <calculator/protocol/Operation.h>


int main(int argc, char **argv) {
    if (argc != 3) {
        printHelp();
        return -1;
    }
    sockaddr_in peer{};
    peer.sin_family = AF_INET;
    try {
        peer.sin_port = htons((uint16_t) std::stoul(argv[2]));
    } catch (const std::exception &e) {
        printHelp();
        return -1;
    }
    peer.sin_addr.s_addr = inet_addr(argv[1]);

    int socket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (socket < 0) {
        std::cerr << "Socket error: " << strerror(errno) << std::endl;
        return -1;
    }

    if (connect(socket, (sockaddr *) &peer, sizeof(peer)) != 0) {
        std::cerr << "Unable to connect: " << strerror(errno) << std::endl;
        return -1;
    }

    std::thread receiveThread([socket]() {
        while (true) {
            uint8_t bytesToBeReceived;
            ssize_t bytesReceived = recv(socket, &bytesToBeReceived, 1, 0);

            if (bytesReceived == -1 || bytesReceived == 0) {
                std::cerr << "Unable to receive: " << strerror(errno) << std::endl;
                break;
            }

            auto bytes = new uint8_t[bytesToBeReceived + 1];
            bytes[0] = bytesToBeReceived;

            bytesReceived = receiveNBytes(socket, bytesToBeReceived, &bytes[1]);
            if (bytesReceived == -1 || bytesReceived == 0) {
                std::cerr << "Unable to receive: " << strerror(errno) << std::endl;
                break;
            }

            auto response = Message::of(bytes);
            switch (response->type()) {
            case MessageType::MATH_RESPONSE:
                switch (MathResponse::typeOf(response->data())) {
                case MathResponseType::FAST_OPERATION_RESULT: {
                    std::cout << MathResponse::resultOf(response->data()) << std::endl;
                    break;
                }
                case MathResponseType::BAD_OPERATION:
                    std::cout << "Bad operation" << std::endl;
                    break;
                case MathResponseType::HARD_OPERATION_SUBMITTED:
                    std::cout << "Operation submitted" << std::endl;
                    break;
                default:
                    break;
                }
                break;
            case MessageType::SERVER_INITIATED_REQUEST:
                std::cout << bytesAsInt64(response->data()) << std::endl;
                break;
            default:
                break;
            }
            delete response;
            delete[] bytes;
        }
    });

    while (true) {
        std::string instruction;
        std::getline(std::cin, instruction);

        auto request = requestWithInstruction(instruction);

        if (request == nullptr) {
            std::cerr << "Invalid instruction" << std::endl;
            continue;
        }

        uint8_t *messageBytes = request->toBytes();
        if (send(socket, (const void *) messageBytes, request->size(), 0) <= 0) {
            std::cerr << "Unable to send: " << strerror(errno) << std::endl;
            break;
        }
        delete[] messageBytes;
        delete request;
    }
    receiveThread.join();

    return 0;
}

Message *requestWithInstruction(const std::string &instruction) {
    if (instruction == "kill me") {
        auto data = new uint8_t;
        *data = 0x00; // TODO: enum? and self coding/decoding class
        return new Message(MessageType::CONTROL_REQUEST, 1, data);
    }

    std::vector<std::string> operators = {"+", "-", "*", "/", "v", "!"};
    Operation *operation = nullptr;
    for (const auto &op : operators) {
        auto idx = instruction.find_last_of(op);
        if (idx == std::string::npos) {
            continue;
        }
        if (op == "+") {
            int64_t operand1;
            int64_t operand2;
            try {
                operand1 = std::stol(instruction.substr(0, idx));
                operand2 = std::stol(instruction.substr(idx + 1));
            } catch (const std::exception &e) {
                return nullptr;
            }
            operation = new Operation(OperationType::ADDITION, operand1, operand2);
            break;
        } else if (op == "-") {
            int64_t operand1;
            int64_t operand2;
            try {
                operand1 = std::stol(instruction.substr(0, idx));
                operand2 = std::stol(instruction.substr(idx + 1));
            } catch (const std::exception &e) {
                return nullptr;
            }
            operation = new Operation(OperationType::SUBTRACTION, operand1, operand2);
            break;
        } else if (op == "*") {
            int64_t operand1;
            int64_t operand2;
            try {
                operand1 = std::stol(instruction.substr(0, idx));
                operand2 = std::stol(instruction.substr(idx + 1));
            } catch (const std::exception &e) {
                return nullptr;
            }
            operation = new Operation(OperationType::MULTIPLICATION, operand1, operand2);
            break;
        } else if (op == "/") {
            int64_t operand1;
            int64_t operand2;
            try {
                operand1 = std::stol(instruction.substr(0, idx));
                operand2 = std::stol(instruction.substr(idx + 1));
            } catch (const std::exception &e) {
                return nullptr;
            }
            operation = new Operation(OperationType::DIVISION, operand1, operand2);
            break;
        } else if (op == "v") {
            int64_t operand1;
            try {
                operand1 = std::stol(instruction.substr(idx + 1));
            } catch (const std::exception &e) {
                return nullptr;
            }
            operation = new Operation(OperationType::SQUARE_ROOT, operand1);
            break;
        } else if (op == "!") {
            int64_t operand1;
            try {
                operand1 = std::stol(instruction.substr(0, idx));
            } catch (const std::exception &e) {
                return nullptr;
            }
            operation = new Operation(OperationType::FACTORIAL, operand1);
            break;
        } else {
            return nullptr;
        }
    }

    if (operation == nullptr) {
        return nullptr;
    }

    return new Message(MessageType::MATH_REQUEST, operation->nBytes(), operation->toBytes());
}

void printHelp() {
    std::cout << help << std::endl;
}
