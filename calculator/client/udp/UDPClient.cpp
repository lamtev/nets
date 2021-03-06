//
// Created by anton.lamtev on 30/10/2018.
//

#include "UDPClient.h"

#include <iostream>
#include <vector>
#include <unordered_map>
#include <thread>
#include <cstring>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <unistd.h>

#include <calculator/protocol/Message.h>
#include <calculator/protocol/Operation.h>
#include <calculator/protocol/MathResponse.h>
#include <calculator/protocol/NumberedMessage.h>
#include <calculator/protocol/BitsUtils.h>

#include <nets_lib/receivenbytes.h>


static const int MAXIMUM_ATTEMPTS_TO_RECEIVE_ACK = 5;

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

int main(int argc, char **argv) {
    UDPClient client("127.0.0.1", 1234);
    client.start();

    return 0;
}

UDPClient::UDPClient(const char *addr, int port) :
        addr(addr),
        port(port),
        socket(0),
        waitingForAck(false),
        ackReceived(false),
        responseReceived(false),
        messageCounter(0) {}

void UDPClient::start() {
    sockaddr_in peer{};
    peer.sin_family = AF_INET;
    peer.sin_port = htons((uint16_t) port);
    peer.sin_addr.s_addr = inet_addr(addr);

    socket = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (socket < 0) {
        std::cerr << "Socket error: " << strerror(errno) << std::endl;
        return;
    }

    unsigned int peerLen = sizeof(peer);

    std::thread receiveThread([this, peer, peerLen]() {
        while (true) {
            auto receivedBytes = new uint8_t[65507];
            ssize_t receivedBytesSize = recvfrom(socket, receivedBytes, 65507, 0, nullptr, nullptr);
            if (receivedBytesSize == -1) {
                std::cout << "Unable to recvfrom" << std::endl;
                break;
            }

            auto response = NumberedMessage::of(receivedBytes);
            switch (response->message()->type()) {
                case MessageType::ACK:
                    ackReceived = true;
                    std::cout << "Ack " << response->number() << " received" << std::endl;
                    break;
                case MessageType::MATH_RESPONSE: {
                    if (waitingForAck) {
                        break;
                    }
                    std::string msg;
                    switch (MathResponse::typeOf(response->message()->data())) {
                        case MathResponseType::FAST_OPERATION_RESULT: {
                            msg = std::to_string(MathResponse::resultOf(response->message()->data()));
                            break;
                        }
                        case MathResponseType::BAD_OPERATION:
                            msg = "Bad operation";
                            break;
                        case MathResponseType::HARD_OPERATION_SUBMITTED:
                            msg = "Operation submitted";
                            break;
                        default:
                            msg = "Unknown response";
                            break;
                    }
                    std::cout << "Response received" << std::endl;
                    std::cout << msg << std::endl;

                    auto ack = new NumberedMessage(response->number(), new Message(MessageType::ACK, 0, nullptr));
                    auto ackBytes = ack->toBytes();
                    if (sendto(socket, (const void *) ackBytes, ack->size(), 0, (sockaddr *) &peer, peerLen) < 0) {
                        std::cerr << "Unable to send: " << strerror(errno) << std::endl;
                        break;
                    }
                    std::cout << "Ack " << response->number() << " sent" << std::endl;
                    delete[] ackBytes;
                    delete ack;

                    responseReceived = true;
                    break;
                }
                case MessageType::SERVER_INITIATED_REQUEST: {
                    if (waitingForAck) {
                        break;
                    }
                    std::cout << "Hard operation result received" << std::endl;
                    std::cout << bytesAsInt64(response->message()->data()) << std::endl;
                    auto ack = new NumberedMessage(response->number(), new Message(MessageType::ACK, 0, nullptr));
                    auto ackBytes = ack->toBytes();
                    if (sendto(socket, (const void *) ackBytes, ack->size(), 0, (sockaddr *) &peer, peerLen) < 0) {
                        std::cerr << "Unable to send: " << strerror(errno) << std::endl;
                        break;
                    }
                    std::cout << "Ack " << response->number() << " sent" << std::endl;
                    delete[] ackBytes;
                    delete ack;
                    break;
                }
                default:
                    break;
            }
            delete response;
            delete[] receivedBytes;
        }
    });

    while (true) {
        std::cout << "Type a command" << std::endl;
        std::string instruction;
        std::getline(std::cin, instruction);

        if (instruction == "exit") {
            stop();
            break;
        }

        if (instruction == "sh") {
            std::vector<uint64_t> nums = {0, 3, 2, 1};
            sendFakeMessages(peer, peerLen, nums);
            continue;
        } else if (instruction == "dup") {
            std::vector<uint64_t> nums = {0, 0, 1, 1};
            sendFakeMessages(peer, peerLen, nums);
            continue;
        } else if (instruction == "los") {
            std::vector<uint64_t> nums = {0, 2, 4, 6};
            sendFakeMessages(peer, peerLen, nums);
            continue;
        }

        auto request = requestWithInstruction(instruction);

        if (request == nullptr) {
            std::cerr << "Invalid instruction" << std::endl;
            continue;
        }

        auto *numberedRequest = new NumberedMessage(messageCounter++, request);
        uint8_t *numberedRequestBytes = numberedRequest->toBytes();

        int attemptsToReceiveAck = 0;
        while (!ackReceived && attemptsToReceiveAck++ < MAXIMUM_ATTEMPTS_TO_RECEIVE_ACK) {
            if (sendto(socket, (const void *) numberedRequestBytes, numberedRequest->size(), 0, (sockaddr *) &peer,
                       peerLen) < 0) {
                std::cerr << "Unable to send: " << strerror(errno) << std::endl;
                break;
            }
            std::cout << "Request sent" << std::endl;
            std::cout << "Waiting for ack " << numberedRequest->number() << " ..." << std::endl;
            waitingForAck = true;
            using namespace std::chrono;
            auto timestamp = high_resolution_clock::now();
            while (!ackReceived && duration_cast<seconds>(high_resolution_clock::now() - timestamp).count() < 1);
        }
        delete[] numberedRequestBytes;
        delete numberedRequest;

        ackReceived = false;
        waitingForAck = false;

        while (!responseReceived && attemptsToReceiveAck < MAXIMUM_ATTEMPTS_TO_RECEIVE_ACK);
        responseReceived = false;
    }
    receiveThread.join();
}

void UDPClient::stop() {
    shutdown(socket, SHUT_RDWR);
    close(socket);
}

void UDPClient::sendFakeMessages(const sockaddr_in &peer, socklen_t peerLen, const std::vector<uint64_t> &nums) {
    std::vector<Message *> v = {
            requestWithInstruction("5+5"),
            requestWithInstruction("5*5"),
            requestWithInstruction("5-5"),
            requestWithInstruction("5/5")
    };

    int i = 0;
    for (auto request : v) {
        auto *numberedRequest = new NumberedMessage(nums[i++], request);
        uint8_t *numberedRequestBytes = numberedRequest->toBytes();

        int attemptsToReceiveAck = 0;
        while (!ackReceived && attemptsToReceiveAck++ < MAXIMUM_ATTEMPTS_TO_RECEIVE_ACK) {
            if (sendto(socket, (const void *) numberedRequestBytes, numberedRequest->size(), 0, (sockaddr *) &peer,
                       peerLen) < 0) {
                std::cerr << "Unable to send: " << strerror(errno) << std::endl;
                break;
            }
            std::cout << "Request sent" << std::endl;
            std::cout << "Waiting for ack " << numberedRequest->number() << " ..." << std::endl;
            waitingForAck = true;
            using namespace std::chrono;
            auto timestamp = high_resolution_clock::now();
            while (!ackReceived && duration_cast<seconds>(high_resolution_clock::now() - timestamp).count() < 1);
        }
        delete[] numberedRequestBytes;
        delete numberedRequest;

        ackReceived = false;
        waitingForAck = false;

        using namespace std::chrono;
        auto timestamp = high_resolution_clock::now();

        while (!responseReceived &&
               attemptsToReceiveAck < MAXIMUM_ATTEMPTS_TO_RECEIVE_ACK &&
               duration_cast<seconds>(high_resolution_clock::now() - timestamp).count() < 1);
        responseReceived = false;
    }

    for (auto request : v) {
        delete request;
    }
}
