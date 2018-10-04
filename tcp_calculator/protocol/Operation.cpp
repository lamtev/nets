//
// Created by anton.lamtev on 30.09.2018.
//

#include "Operation.h"
#include "BitsUtils.h"


Operation::Operation(OperationType type, int64_t operand1, int64_t operand2) noexcept {
    switch (type) {
    case OperationType::ADDITION:
    case OperationType::SUBTRACTION:
    case OperationType::MULTIPLICATION:
    case OperationType::DIVISION:
        _type = type;
        _operand1 = operand1;
        _operand2 = operand2;
        _nBytes = TWO_OPERAND_OPERATION_LENGTH;
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
        _nBytes = ONE_OPERAND_OPERATION_LENGTH;
        _isValid = true;
        break;
    default:
        _isValid = false;
    }
}

Operation *Operation::of(uint8_t *bytes) {
    auto operationType = OperationType(bytes[0]);
    switch (operationType) {
    case OperationType::ADDITION:
    case OperationType::SUBTRACTION:
    case OperationType::MULTIPLICATION:
    case OperationType::DIVISION:
        return new Operation(operationType, bytesAsInt64(&bytes[1]), bytesAsInt64(&bytes[9]));
    case OperationType::SQUARE_ROOT:
    case OperationType::FACTORIAL:
        return new Operation(operationType, bytesAsInt64(&bytes[1]));
    default:
        return nullptr;
    }
}

uint8_t *Operation::toBytes() const {
    if (!_isValid) {
        return nullptr;
    }

    switch (_type) {
    case OperationType::ADDITION:
    case OperationType::SUBTRACTION:
    case OperationType::MULTIPLICATION:
    case OperationType::DIVISION: {
        auto *bytes = new unsigned char[TWO_OPERAND_OPERATION_LENGTH];
        bytes[0] = uint8_t(_type);
        int64AsBytes(_operand1, &bytes[1]);
        int64AsBytes(_operand2, &bytes[9]);

        return bytes;
    }
    case OperationType::SQUARE_ROOT:
    case OperationType::FACTORIAL: {
        auto *bytes = new unsigned char[ONE_OPERAND_OPERATION_LENGTH];
        bytes[0] = uint8_t(_type);
        int64AsBytes(_operand1, &bytes[1]);

        return bytes;
    }
    default:
        return nullptr;
    }
}

uint8_t Operation::nBytes() const noexcept {
    return _nBytes;
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

    std::string op;
    switch (_type) {
    case OperationType::ADDITION:
        op = "+";
        break;
    case OperationType::SUBTRACTION:
        op = "-";
        break;
    case OperationType::MULTIPLICATION:
        op = "*";
        break;
    case OperationType::DIVISION:
        op = "/";
        break;
    case OperationType::SQUARE_ROOT:
        return std::string("Operation{ ") + "√" + std::to_string(_operand1) + " }";
    case OperationType::FACTORIAL:
        return std::string("Operation{ ") + std::to_string(_operand1) + "! }";
    }
    return std::string("Operation{ ") + std::to_string(_operand1) + op + std::to_string(_operand2) + " }";
}
