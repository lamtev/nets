//
// Created by anton.lamtev on 29.09.2018.
//

#ifndef NETS_OPERATION_H
#define NETS_OPERATION_H

#include <string>
#include <cstdint>


enum class OperationType : uint8_t {
    ADDITION = 0x00,
    SUBTRACTION = 0x01,
    MULTIPLICATION = 0x02,
    DIVISION = 0x03,
    SQUARE_ROOT = 0x04,
    FACTORIAL = 0x05,
};

class Operation {
 private:
    OperationType _type;
    int64_t _operand1;
    int64_t _operand2;
    bool _isValid;
    uint8_t _nBytes;
    static const uint8_t TWO_OPERAND_OPERATION_LENGTH = 2 * sizeof(int64_t) + 1;
    static const uint8_t ONE_OPERAND_OPERATION_LENGTH = sizeof(int64_t) + 1;

 public:
    Operation(OperationType type, int64_t operand1, int64_t operand2) noexcept;

    Operation(OperationType type, int64_t operand1) noexcept;

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
    static Operation *of(uint8_t *bytes);

    uint8_t *toBytes() const;

    uint8_t nBytes() const noexcept;

    bool isValid() const noexcept;

    OperationType type() const noexcept;

    int64_t operand1() const noexcept;

    int64_t operand2() const noexcept;

    bool equals(const Operation &operation) const noexcept;

    std::string toString() const noexcept;
};


#endif //NETS_OPERATION_H
