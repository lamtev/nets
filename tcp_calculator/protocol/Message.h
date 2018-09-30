//
// Created by anton.lamtev on 30.09.2018.
//

#ifndef NETS_MESSAGE_H
#define NETS_MESSAGE_H

#include <sys/types.h>

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
    u_int8_t _dataSize;
    u_int8_t *_data;

 public:
    Message(MessageType type, u_int8_t dataSize, u_int8_t *data) : _type(type), _dataSize(dataSize), _data(data) {
    }

    MessageType type() const {
        return _type;
    }

    u_int8_t dataSize() const {
        return _dataSize;
    }

    u_int8_t *data() const {
        return _data;
    }

};


#endif //NETS_MESSAGE_H
