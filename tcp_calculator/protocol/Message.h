//
// Created by anton.lamtev on 30.09.2018.
//

#ifndef NETS_MESSAGE_H
#define NETS_MESSAGE_H

#include <cstdint>


enum class MessageType {
    MATH_REQUEST = 0,
    MATH_RESPONSE,
    CONTROL_REQUEST,
    CONTROL_RESPONSE,
    SERVER_INITIATED_REQUEST,
};

class Message {
 private:
    MessageType _type;
    uint8_t _dataSize;
    uint8_t *_data;

 public:
    Message(MessageType type, uint8_t dataSize, uint8_t *data);

    static Message *of(uint8_t *bytes);

    uint8_t *toBytes();

    MessageType type() const;

    uint8_t dataSize() const;

    uint8_t *data() const;

};


#endif //NETS_MESSAGE_H
