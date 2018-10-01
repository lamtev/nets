//
// Created by anton.lamtev on 30.09.2018.
//

#include "Operation.h"

void longAsBytes(int64_t _long, uint8_t *bytes) {
    for (auto i = 0; i < 8; ++i) {
        bytes[7 - i] = (unsigned char) (_long >> (i << 3));
    }
}

long bytesAsLong(const uint8_t *bytes) {
    return (((int64_t) bytes[0]) << 56 |
            ((int64_t) bytes[1]) << 48 |
            ((int64_t) bytes[2]) << 40 |
            ((int64_t) bytes[3]) << 32 |
            ((int64_t) bytes[4]) << 24 |
            ((int64_t) bytes[5]) << 16 |
            ((int64_t) bytes[6]) << 8 |
            ((int64_t) bytes[7]));
}

Operation *Operation::of(uint8_t *bytes) {
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

uint8_t *Operation::toBytes() const {
    if (!_isValid) {
        return nullptr;
    }

    switch (_type) {
    case OperationType::ADDITION: {
        auto *bytes = new unsigned char[17];
        bytes[0] = 0x00;
        longAsBytes(_operand1, &bytes[1]);
        longAsBytes(_operand2, &bytes[9]);

        return bytes;
    }
    case OperationType::SUBTRACTION: {
        auto *bytes = new unsigned char[17];
        bytes[0] = 0x01;
        longAsBytes(_operand1, &bytes[1]);
        longAsBytes(_operand2, &bytes[9]);

        return bytes;
    }
    case OperationType::MULTIPLICATION: {
        auto *bytes = new unsigned char[17];
        bytes[0] = 0x02;
        longAsBytes(_operand1, &bytes[1]);
        longAsBytes(_operand2, &bytes[9]);

        return bytes;
    }
    case OperationType::DIVISION: {
        auto *bytes = new unsigned char[17];
        bytes[0] = 0x03;
        longAsBytes(_operand1, &bytes[1]);
        longAsBytes(_operand2, &bytes[9]);

        return bytes;
    }
    case OperationType::SQUARE_ROOT: {
        auto *bytes = new unsigned char[9];
        bytes[0] = 0x04;
        longAsBytes(_operand1, &bytes[1]);

        return bytes;
    }
    case OperationType::FACTORIAL: {
        auto *bytes = new unsigned char[9];
        bytes[0] = 0x05;
        longAsBytes(_operand1, &bytes[1]);

        return bytes;
    }
    }
}

uint8_t Operation::nBytes() const noexcept {
    return _nBytes;
}

Operation::Operation(OperationType type, int64_t operand1, int64_t operand2) noexcept {
    switch (type) {
    case OperationType::ADDITION:
    case OperationType::SUBTRACTION:
    case OperationType::MULTIPLICATION:
    case OperationType::DIVISION:
        _type = type;
        _operand1 = operand1;
        _operand2 = operand2;
        _nBytes = 17;
        _isValid = true;
        break;
    default:
        _isValid = false;
    }
}

Operation::Operation(OperationType type, int64_t operand1) noexcept {
    switch (type) {
    case OperationType::SQUARE_ROOT:
    case OperationType::FACTORIAL:
        _type = type;
        _operand1 = operand1;
        _operand2 = 0;
        _nBytes = 9;
        _isValid = true;
        break;
    default:
        _isValid = false;
    }
}

bool Operation::isValid() const noexcept {
    return _isValid;
}

OperationType Operation::type() const noexcept {
    return _type;
}

int64_t Operation::operand1() const noexcept {
    return _operand1;
}

int64_t Operation::operand2() const noexcept {
    return _operand2;
}

bool Operation::equals(const Operation &operation) const noexcept {
    return _isValid && operation._isValid &&
            operation._type == _type &&
            operation._operand1 == _operand1 &&
            operation._operand2 == _operand2;
}

std::string Operation::toString() const noexcept {
    if (!_isValid) {
        return "";
    }
    switch (_type) {
    case OperationType::ADDITION:
        return std::string("Operation{ ") + std::to_string(_operand1) + " + " + std::to_string(_operand2) + " }";
    case OperationType::SUBTRACTION:
        return std::string("Operation{ ") + std::to_string(_operand1) + " - " + std::to_string(_operand2) + " }";
    case OperationType::MULTIPLICATION:
        return std::string("Operation{ ") + std::to_string(_operand1) + " * " + std::to_string(_operand2) + " }";
    case OperationType::DIVISION:
        return std::string("Operation{ ") + std::to_string(_operand1) + " / " + std::to_string(_operand2) + " }";
    case OperationType::SQUARE_ROOT:
        return std::string("Operation{ ") + "√" + std::to_string(_operand1) + " }";
    case OperationType::FACTORIAL:
        return std::string("Operation{ ") + std::to_string(_operand1) + "! }";
    }
}
