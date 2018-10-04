//
// Created by anton.lamtev on 02.10.2018.
//

#include "Message.h"

#include <memory>


Message::Message(MessageType type, uint8_t dataSize, uint8_t *data) : _type(type), _dataSize(dataSize), _data(data) {}

Message *Message::of(uint8_t *bytes) {
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

uint8_t *Message::toBytes() {
    auto bytes = new uint8_t[2 + _dataSize];
    switch (_type) {
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
    bytes[0] = static_cast<uint8_t>(_dataSize + 1);
    std::memcpy(&bytes[2], _data, _dataSize);

    return bytes;
}

MessageType Message::type() const {
    return _type;
}

uint8_t Message::dataSize() const {
    return _dataSize;
}

uint8_t *Message::data() const {
    return _data;
}
