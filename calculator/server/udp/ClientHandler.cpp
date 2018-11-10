//
// Created by anton.lamtev on 09/11/2018.
//

#include "ClientHandler.h"

#include <iostream>
#include <thread>
#include <algorithm>
#include <cmath>

#include <calculator/protocol/Message.h>
#include <calculator/protocol/Operation.h>
#include <calculator/protocol/MathResponse.h>
#include <calculator/protocol/NumberedMessage.h>
#include <calculator/protocol/MathUtils.h>
#include <calculator/protocol/BitsUtils.h>


Message *ClientHandler::handleRequest(NumberedMessage *request, Operation **hardOperation) {
    MessageType responseType;
    uint8_t dataSize;
    uint8_t *data;
    auto message = request->message();
    switch (message->type()) {
        case MessageType::ACK:
            if (request->number() == responseNumber -1) {
                std::cout << "Ack " << request->number() << " received" << std::endl;
                ackReceived = true;
            }
            return nullptr;
        case MessageType::MATH_REQUEST: {
            auto operation = Operation::of(message->data());
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
                    *hardOperation = new Operation(operation->type(), operation->operand1());
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
        default:
            return nullptr;
    }
    delete message;

    return new Message(responseType, dataSize, data);
}

ClientHandler::ClientHandler(const SockAddr &addr) :
        sockAddr(addr),
        ackReceived(false),
        expectedRequestNumber(0),
        responseNumber(0),
        threads(),
        threadsMutex() {}

ClientHandler::~ClientHandler() {
    std::lock_guard<std::mutex> lock(threadsMutex);
    std::for_each(threads.cbegin(), threads.cend(), [](std::thread *thread) {
        thread->join();
        delete thread;
    });
}

void ClientHandler::submit(
        uint8_t *data,
        size_t size,
        const std::function<void(uint64_t ackNumber)> &ackCallback,
        const std::function<void(uint8_t *data, size_t size, uint64_t msgNumber)> &responseCallback) {
    auto submitThread = new std::thread([data, ackCallback, responseCallback, this]() {
        auto numberedRequest = NumberedMessage::of(data);
        delete[] data;
        Operation *hardOperation = nullptr;
        auto response = handleRequest(numberedRequest, &hardOperation);

        if (response == nullptr) {
            //TODO: delete objects
            return;
        }

        if (numberedRequest->number() > expectedRequestNumber) {
            return;
        } else if (numberedRequest->number() < expectedRequestNumber) {
            ackCallback(numberedRequest->number());
            return;
        } else {
            ackCallback(expectedRequestNumber++);
        }

        auto numberedResponse = new NumberedMessage(responseNumber++, response);
        delete numberedRequest;

        auto bytes = numberedResponse->toBytes();

        responseCallback(bytes, numberedResponse->size(), numberedResponse->number());
        delete numberedResponse;
        delete[] bytes;

        delete response;

        if (hardOperation) {
            std::this_thread::sleep_for(std::chrono::seconds(20));
            int64_t res;
            switch (hardOperation->type()) {
                case OperationType::FACTORIAL:
                    res = factorial(static_cast<uint64_t>(hardOperation->operand1()));
                    break;
                case OperationType::SQUARE_ROOT:
                    res = static_cast<int64_t>(sqrt(hardOperation->operand1()));
                    break;
                default:
                    //never happens
                    return;
            }
            auto *data = new uint8_t[8];
            int64AsBytes(res, data);
            Message message(MessageType::SERVER_INITIATED_REQUEST, 8, data);
            auto numberedMessage = new NumberedMessage(responseNumber, &message);
            auto bytes = numberedMessage->toBytes();
            delete[] data;
            responseCallback(bytes, numberedMessage->size(), responseNumber++);
            delete[] bytes;
            delete numberedMessage;
        }

    });

    std::lock_guard<std::mutex> lock(threadsMutex);
    threads.push_back(submitThread);
}
