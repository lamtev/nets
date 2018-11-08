//
// Created by anton.lamtev on 02/11/2018.
//

#ifndef NETS_NUMBEREDMESSAGE_H
#define NETS_NUMBEREDMESSAGE_H


#include <cstdint>

#include "Message.h"

class NumberedMessage {
private:
    uint64_t _number;
    Message *_message;

public:
    NumberedMessage(uint64_t number, Message *message);

    uint64_t number() const;

    Message *message() const;

    uint8_t messageSize() const;

    static NumberedMessage *of(uint8_t *bytes);

    uint8_t *toBytes();

    uint8_t size() const;

};

#endif //NETS_NUMBEREDMESSAGE_H
