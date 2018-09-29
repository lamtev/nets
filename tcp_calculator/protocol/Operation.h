//
// Created by anton.lamtev on 29.09.2018.
//

#ifndef NETS_OPERATION_H
#define NETS_OPERATION_H

#include <string>

enum class OperationType {
    ADDITION = 0,
    SUBTRACTION,
    MULTIPLICATION,
    DIVISION,
    SQUARE_ROOT,
    FACTORIAL
};

class Operation {
 private:
    OperationType _type;
    long _operand1;
    long _operand2;
    bool _isValid;

 public:
    Operation(OperationType type, long operand1, long operand2) {
        switch (type) {
        case OperationType::ADDITION:
        case OperationType::SUBTRACTION:
        case OperationType::MULTIPLICATION:
        case OperationType::DIVISION:
            _type = type;
            _operand1 = operand1;
            _operand2 = operand2;
            _isValid = true;
            break;
        default:
            _isValid = false;
        }
    }

    Operation(OperationType type, long operand1) {
        switch (type) {
        case OperationType::SQUARE_ROOT:
        case OperationType::FACTORIAL:
            _type = type;
            _operand1 = operand1;
            _operand2 = 0;
            _isValid = true;
            break;
        default:
            _isValid = false;
        }
    }

    bool isValid() const {
        return _isValid;
    }

    OperationType type() const {
        return _type;
    }

    long operand1() const {
        return _operand1;
    }

    long operand2() const {
        return _operand2;
    }

    bool equals(const Operation &operation) const {
        return operation._type == _type &&
                operation._operand1 == _operand1 &&
                operation._operand2 == _operand2;
    }

    std::string toString() const {
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

};


#endif //NETS_OPERATION_H
