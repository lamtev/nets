//
// Created by anton.lamtev on 30/10/2018.
//

#include <iostream>
#include <netinet/in.h>

#include <calculator/protocol/Message.h>
#include <calculator/protocol/NumberedMessage.h>
#include <calculator/protocol/Operation.h>
#include <calculator/protocol/MathResponse.h>
#include <calculator/protocol/BitsUtils.h>
#include <nets_lib/send.h>

Message *handleRequest(Message *request, int socket) {
    MessageType responseType;
    uint8_t dataSize;
    uint8_t *data;
    switch (request->type()) {
        case MessageType::ACK:
            return nullptr;
        case MessageType::MATH_REQUEST: {
            auto operation = Operation::of(request->data());
            std::cout << operation->toString() << std::endl;
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
//                    submitHardOperation(*operation, socket);
                    break;
                default:
                    result = 0;
                    mathResponseType = MathResponseType::BAD_OPERATION;
                    break;
            }
            delete operation;
            std::cout << result << std::endl;
            MathResponse mathResponse = MathResponse(mathResponseType, result);
            dataSize = mathResponse.size();
            data = mathResponse.toBytes();
            break;
        }
        case MessageType::CONTROL_REQUEST: {
            if (*request->data() == 0x00) { //0x00 for CONTROL_REQUEST means "kill me". It would be better if i will add an enum
//                std::shared_lock<std::shared_mutex> lock(clientsMutex);
//                auto toBeRemoved = std::find_if(clients.cbegin(),
//                                                clients.cend(),
//                                                [socket](const ClientSession &client) -> bool {
//                                                    return client.socket() == socket;
//                                                });
//                if (toBeRemoved != clients.end()) {
//                    closeSocket(toBeRemoved->socket());
//                    clients.erase(toBeRemoved);
//                } else if (delegate != nullptr) {
//                    delegate->netDidFailWithError(this, ServerNetError::KILL_CLIENT_ERROR);
//                }
//                lock.unlock();
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

std::atomic<uint64_t> cnt = 0;
std::atomic<bool> ackReceived = false;

int main(int argc, char **argv) {
    sockaddr_in local{};
    local.sin_port = htons(1234);
    local.sin_addr.s_addr = htonl(INADDR_ANY);
    local.sin_family = AF_INET;

    int socket = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (socket < 0) {
        std::cerr << "Socket error: " << strerror(errno) << std::endl;
        return 1;
    }

    if (bind(socket, (sockaddr *) &local, sizeof(local)) != 0) {
        std::cerr << "Unable to bind: " << strerror(errno) << std::endl;
        return 1;
    }

    sockaddr_in peer{};
    unsigned int peerlen;
    uint8_t buf[65507];
    while (true) {
        peerlen = sizeof(peer);
        ssize_t received = recvfrom(socket, buf, sizeof(buf), 0, (sockaddr *) &peer, &peerlen);
        if (received < 0) {
            std::cerr << "Unable to recvfrom: " << strerror(errno) << std::endl;
            return 1;
        } else if (received == 0) {
            std::cout << "0 bytes received" << std::endl;
        }

        auto numberedRequest = NumberedMessage::of(buf);
        auto request = numberedRequest->message();
        auto response = handleRequest(request, socket);
        delete request;

        if (!response) {
            std::cout << "Ack " << numberedRequest->number() << " received" << std::endl;
            continue;
        }

        std::cout << "Request received" << std::endl;

        auto ack = new NumberedMessage(numberedRequest->number(), new Message(MessageType::ACK, 0, nullptr));
        auto ackBytes = ack->toBytes();
        if (sendto(socket, ackBytes, ack->size(), 0, (sockaddr *) &peer, peerlen) < 0) {
            std::cerr << "Unable to sendto: " << strerror(errno) << std::endl;
            return 1;
        }
        delete ack;
        delete[] ackBytes;

        std::cout << "Ack " << numberedRequest->number() << " sent" << std::endl;


        auto numberedResponse = new NumberedMessage(numberedRequest->number(), response);
        auto numberedResponseBytes = numberedResponse->toBytes();

        delete numberedRequest;

        if (sendto(socket, numberedResponseBytes, numberedResponse->size(), 0, (sockaddr *) &peer, peerlen) < 0) {
            std::cerr << "Unable to sendto: " << strerror(errno) << std::endl;
            return 1;
        }

        std::cout << "Response sent" << std::endl;

        delete[] numberedResponseBytes;
        delete numberedResponse;
        delete response;
    }
    return 0;
}