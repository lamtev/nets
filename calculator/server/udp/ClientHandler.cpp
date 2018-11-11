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


ClientHandler::ClientHandler(const SockAddr &addr, uint64_t id) :
        sockAddr(addr),
        id(id),
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
        const Callback &ackCallback,
        const Callback &responseCallback) {
    auto submitThread = new std::thread([data, ackCallback, responseCallback, this]() {
        auto request = NumberedMessage::of(data);
        delete[] data;
        Operation *hardOperation = nullptr;
        auto response = handleRequest(request, &hardOperation);

        if (response == nullptr) {
            delete request;
            return;
        }

        uint8_t *ackData;
        size_t ackSize;

        if (request->number() > expectedRequestNumber) {
            std::cout << "[Client id=" << id << "]\t" << "Request " << request->number() << " dropped" << std::endl;
            std::cout << "[Client id=" << id << "]\t" << "Ack " << request->number() << " is not sent" << std::endl;
            delete request;
            delete hardOperation;
            return;
        } else if (request->number() < expectedRequestNumber) {
            std::cout << "[Client id=" << id << "]\t" << "Request " << request->number() << " dropped" << std::endl;
            ackWithNumber(request->number(), &ackData, &ackSize);
            ackCallback(ackData, ackSize, request->number());
            delete[] ackData;
            delete request;
            delete hardOperation;
            return;
        } else {
            ackWithNumber(expectedRequestNumber, &ackData, &ackSize);
            ackCallback(ackData, ackSize, expectedRequestNumber++);
            delete[] ackData;
        }

        auto numberedResponse = new NumberedMessage(responseNumber++, response);
        delete request;

        auto bytes = numberedResponse->toBytes();

        responseCallback(bytes, numberedResponse->size(), numberedResponse->number());

        delete[] bytes;
        delete numberedResponse;
        delete response;

        if (hardOperation) {
            handleHardOperation(hardOperation, responseCallback);
        }
    });

    std::lock_guard<std::mutex> lock(threadsMutex);
    threads.push_back(submitThread);
}

Message *ClientHandler::handleRequest(NumberedMessage *request, Operation **hardOperation) {
    MessageType responseType;
    uint8_t dataSize;
    uint8_t *data;
    auto message = request->message();
    switch (message->type()) {
        case MessageType::ACK:
            if (request->number() == responseNumber - 1) {
                std::cout << "[Client id=" << id << "]\t" << "Ack " << request->number() << " received" << std::endl;
                ackReceived = true;
            }
            return nullptr;
        case MessageType::MATH_REQUEST: {
            std::cout << "[Client id=" << id << "]\t" << "Request " << request->number() << " received" << std::endl;
            auto operation = Operation::of(message->data());
            responseType = MessageType::MATH_RESPONSE;
            int64_t result = 0;
            MathResponseType mathResponseType;
            if (operation) {
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

            }
            delete operation;
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

void ClientHandler::handleHardOperation(
        Operation *hardOperation,
        const Callback &responseCallback) {
    std::this_thread::sleep_for(std::chrono::seconds(20)); //make operation really hard =)
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

void ClientHandler::ackWithNumber(uint64_t number, uint8_t **ackData, size_t *ackSize) {
    auto ack = new NumberedMessage(number, new Message(MessageType::ACK, 0, nullptr));
    auto ackBytes = ack->toBytes();
    *ackData = ackBytes;
    *ackSize = ack->size();
    delete ack;
}
