//
// Created by anton.lamtev on 02.10.2018.
//

#include "Message.h"

#include <memory>


Message::Message(MessageType type, uint8_t dataSize, uint8_t *data) : _type(type), _dataSize(dataSize), _data(data) {}

Message *Message::of(uint8_t *bytes) {
    return new Message(MessageType(bytes[1]), uint8_t(bytes[0] - 1), &bytes[2]);
}

uint8_t *Message::toBytes() {
    auto bytes = new uint8_t[2 + _dataSize];
    bytes[1] = uint8_t(_type);
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

uint8_t Message::size() const {
    return _dataSize + 2 * sizeof(uint8_t);
}
