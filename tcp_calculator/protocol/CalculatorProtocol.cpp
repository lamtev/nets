//
// Created by anton.lamtev on 29.09.2018.
//

#include "CalculatorProtocol.h"

void longAsBytes(long _long, unsigned char *bytes) {
    for (auto i = 0; i < 8; ++i) {
        bytes[7 - i] = static_cast<unsigned char>(_long >> (i << 3));
    }
}

long bytesAsLong(const unsigned char *bytes) {
    return (((long) bytes[0]) << 56 |
            ((long) bytes[1]) << 48 |
            ((long) bytes[2]) << 40 |
            ((long) bytes[3]) << 32 |
            ((long) bytes[4]) << 24 |
            ((long) bytes[5]) << 16 |
            ((long) bytes[6]) << 8 |
            ((long) bytes[7]));
}

unsigned char *CalculatorProtocol::encode(const Operation &operation) {
    if (!operation.isValid()) {
        return nullptr;
    }

    switch (operation.type()) {
    case OperationType::ADDITION: {
        auto *bytes = new unsigned char[17];
        bytes[0] = 0x00;
        longAsBytes(operation.operand1(), &bytes[1]);
        longAsBytes(operation.operand2(), &bytes[9]);

        return bytes;
    }
    case OperationType::SUBTRACTION: {
        auto *bytes = new unsigned char[17];
        bytes[0] = 0x01;
        longAsBytes(operation.operand1(), &bytes[1]);
        longAsBytes(operation.operand2(), &bytes[9]);

        return bytes;
    }
    case OperationType::MULTIPLICATION: {
        auto *bytes = new unsigned char[17];
        bytes[0] = 0x02;
        longAsBytes(operation.operand1(), &bytes[1]);
        longAsBytes(operation.operand2(), &bytes[9]);

        return bytes;
    }
    case OperationType::DIVISION: {
        auto *bytes = new unsigned char[17];
        bytes[0] = 0x03;
        longAsBytes(operation.operand1(), &bytes[1]);
        longAsBytes(operation.operand2(), &bytes[9]);

        return bytes;
    }
    case OperationType::SQUARE_ROOT: {
        auto *bytes = new unsigned char[9];
        bytes[0] = 0x04;
        longAsBytes(operation.operand1(), &bytes[1]);

        return bytes;
    }
    case OperationType::FACTORIAL: {
        auto *bytes = new unsigned char[9];
        bytes[0] = 0x05;
        longAsBytes(operation.operand1(), &bytes[1]);

        return bytes;
    }
    }
}

Operation *CalculatorProtocol::decode(unsigned char *bytes) {
    switch (bytes[0]) {
    case 0x00: {
        return new Operation(OperationType::ADDITION, bytesAsLong(&bytes[1]), bytesAsLong(&bytes[9]));
    }
    case 0x01: {
        return new Operation(OperationType::SUBTRACTION, bytesAsLong(&bytes[1]), bytesAsLong(&bytes[9]));
    }
    case 0x02: {
        return new Operation(OperationType::MULTIPLICATION, bytesAsLong(&bytes[1]), bytesAsLong(&bytes[9]));
    }
    case 0x03: {
        return new Operation(OperationType::DIVISION, bytesAsLong(&bytes[1]), bytesAsLong(&bytes[9]));
    }
    case 0x04: {
        return new Operation(OperationType::SQUARE_ROOT, bytesAsLong(&bytes[1]));
    }
    case 0x05: {
        return new Operation(OperationType::FACTORIAL, bytesAsLong(&bytes[1]));
    }
    default:
        return nullptr;
    }
}
