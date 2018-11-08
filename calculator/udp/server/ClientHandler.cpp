//
// Created by anton.lamtev on 09/11/2018.
//

#include "ClientHandler.h"

#include <iostream>
#include <thread>

#include <calculator/protocol/Message.h>
#include <calculator/protocol/Operation.h>
#include <calculator/protocol/MathResponse.h>
#include <calculator/protocol/NumberedMessage.h>

Message *ClientHandler::handleRequest(Message *request, int socket) {
    MessageType responseType;
    uint8_t dataSize;
    uint8_t *data;
    switch (request->type()) {
        case MessageType::ACK:
            std::cout << "Ack " << " received" << std::endl;
            ackReceived = true;
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
            if (*request->data() ==
                0x00) { //0x00 for CONTROL_REQUEST means "kill me". It would be better if i will add an enum
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

ClientHandler::ClientHandler(const SockAddr &addr) : ackReceived(false) {

}

void ClientHandler::submit(
        uint8_t *data,
        size_t size,
        const std::function<void(uint64_t ackNumber)> &ackCallback,
        const std::function<void(uint8_t *data, size_t size, uint64_t msgNumber)> &responseCallback) {
    //TODO: join thread
    auto submitThread = new std::thread([data, ackCallback, responseCallback, this]() {
        auto numberedRequest = NumberedMessage::of(data);
        auto request = numberedRequest->message();
        auto response = handleRequest(request, 0);
        if (response == nullptr) {
            //TODO: delete objects
            return;
        }
        delete request;

        ackCallback(numberedRequest->number());

        auto numberedResponse = new NumberedMessage(numberedRequest->number(), response);
        delete numberedRequest;

        auto bytes = numberedResponse->toBytes();

        responseCallback(bytes, numberedResponse->size(), numberedResponse->number());
        delete numberedResponse;
        delete[] bytes;
    });
    threads.push_back(submitThread);
}
