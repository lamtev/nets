//
// Created by anton.lamtev on 02/11/2018.
//

#include "NumberedMessage.h"

#include "BitsUtils.h"

#include <memory>


NumberedMessage::NumberedMessage(uint64_t number, Message *message)
        : _number(number), _message(message) {}

uint64_t NumberedMessage::number() const {
    return _number;
}

Message *NumberedMessage::message() const {
    return _message;
}

uint8_t NumberedMessage::messageSize() const {
    return _message->size();
}

NumberedMessage *NumberedMessage::of(uint8_t *bytes) {
    return new NumberedMessage(bytesAsUint64(bytes), Message::of(&bytes[8]));
}

uint8_t *NumberedMessage::toBytes() {
    auto bytes = new uint8_t[size()];
    uint64AsBytes(_number, bytes);
    std::memcpy(&bytes[8], _message->toBytes(), _message->size());

    return bytes;
}

uint8_t NumberedMessage::size() const {
    return sizeof(_number) + _message->size();
}
