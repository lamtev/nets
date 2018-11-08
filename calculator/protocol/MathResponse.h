//
// Created by anton.lamtev on 08.10.2018.
//

#ifndef NETS_MATHRESPONSE_H
#define NETS_MATHRESPONSE_H

#include <stdint.h>


enum class MathResponseType : uint8_t {
    BAD_OPERATION = 0x00,
    FAST_OPERATION_RESULT = 0x01,
    HARD_OPERATION_SUBMITTED = 0x02,
};

class MathResponse {
 private:
    MathResponseType _type;
    int64_t _result;

 public:
    explicit MathResponse(MathResponseType type, int64_t result = 0);
    static MathResponse *of(uint8_t *bytes);
    uint8_t *toBytes();
    MathResponseType type();
    int64_t result();
    uint8_t size();
    static MathResponseType typeOf(uint8_t *bytes);
    static int64_t resultOf(uint8_t *bytes);
};

#endif //NETS_MATHRESPONSE_H
