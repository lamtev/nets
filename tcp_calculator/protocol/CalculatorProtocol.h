//
// Created by anton.lamtev on 29.09.2018.
//

#ifndef NETS_CALCULATORPROTOCOL_H
#define NETS_CALCULATORPROTOCOL_H

#include "Operation.h"

/**
 *
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
class CalculatorProtocol {
 public:
    static unsigned char *encode(const Operation &operation);
    static Operation *decode(unsigned char *bytes);
};

#endif //NETS_CALCULATORPROTOCOL_H
