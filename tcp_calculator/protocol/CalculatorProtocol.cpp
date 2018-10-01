//
// Created by anton.lamtev on 29.09.2018.
//

#include <memory>

#include "Message.h"
#include "CalculatorProtocol.h"

uint8_t *CalculatorProtocol::encode(const Message *message) {
    auto bytes = new uint8_t[2 + message->dataSize()];
    switch (message->type()) {
    case MessageType::MATH_REQUEST:
        bytes[1] = 0x00;
        break;
    case MessageType::MATH_RESPONSE:
        bytes[1] = 0x01;
        break;
    case MessageType::CONTROL_REQUEST:
        bytes[1] = 0x02;
        break;
    case MessageType::CONTROL_RESPONSE:
        bytes[1] = 0x03;
        break;
    case MessageType::SERVER_INITIATED_REQUEST:
        bytes[1] = 0x04;
        break;
    default:
        return nullptr;
    }
    bytes[0] = static_cast<uint8_t>(message->dataSize() + 1);
    std::memcpy(&bytes[2], message->data(), message->dataSize());

    return bytes;
}

Message *CalculatorProtocol::decode(uint8_t *bytes) {
    MessageType messageType;
    switch (bytes[1]) {
    case 0x00:
        messageType = MessageType::MATH_REQUEST;
        break;
    case 0x01:
        messageType = MessageType::MATH_RESPONSE;
        break;
    case 0x02:
        messageType = MessageType::CONTROL_REQUEST;
        break;
    case 0x03:
        messageType = MessageType::CONTROL_RESPONSE;
        break;
    case 0x04:
        messageType = MessageType::SERVER_INITIATED_REQUEST;
        break;
    default:
        return nullptr;
    }

    return new Message(messageType, static_cast<uint8_t>(bytes[0] - 1), &bytes[2]);
}
