//
// Created by anton.lamtev on 30.09.2018.
//

#ifndef NETS_MESSAGE_H
#define NETS_MESSAGE_H

#include <cstdint>


enum class MessageType: uint8_t {
    MATH_REQUEST = 0x00,
    MATH_RESPONSE = 0x01,
    CONTROL_REQUEST = 0x02,
    CONTROL_RESPONSE = 0x03,
    SERVER_INITIATED_REQUEST = 0x04,
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
