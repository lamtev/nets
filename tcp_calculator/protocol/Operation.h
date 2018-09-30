//
// Created by anton.lamtev on 29.09.2018.
//

#ifndef NETS_OPERATION_H
#define NETS_OPERATION_H

#include <string>
#include <cstdint>

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
    int64_t _operand1;
    int64_t _operand2;
    bool _isValid;
    u_int8_t _nBytes;

 public:

    /**
     *
     * Codes:
     *
     * 0x00 - "+"
     * 0x01 - "-"
     * 0x02 - "*"
     * 0x03 - "/"
     * 0x04 - "!"
     * 0x05 - "√"
     *
     */
    static Operation *of(u_int8_t *bytes);

    u_int8_t *toBytes() const;

    u_int8_t nBytes() const noexcept;

    bool isValid() const noexcept;

    OperationType type() const noexcept;

    int64_t operand1() const noexcept;

    int64_t operand2() const noexcept;

    bool equals(const Operation &operation) const noexcept;

    std::string toString() const noexcept;

 private:
    Operation(OperationType type, int64_t operand1, int64_t operand2) noexcept;

    Operation(OperationType type, int64_t operand1) noexcept;
};


#endif //NETS_OPERATION_H
