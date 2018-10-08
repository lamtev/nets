//
// Created by anton.lamtev on 08.10.2018.
//

#include "MathResponse.h"
#include "BitsUtils.h"


MathResponse::MathResponse(MathResponseType type, int64_t result) : _type(type), _result(result) {}

MathResponse *MathResponse::of(uint8_t *bytes) {
    auto type = MathResponseType(bytes[0]);
    switch (type) {
    case MathResponseType::BAD_OPERATION:
    case MathResponseType::HARD_OPERATION_SUBMITTED:
        return new MathResponse(type);
    case MathResponseType::FAST_OPERATION_RESULT:
        return new MathResponse(type, bytesAsInt64(&bytes[1]));
    default:
        return nullptr;
    }
}

uint8_t *MathResponse::toBytes() {
    uint8_t *bytes;
    switch (_type) {
    case MathResponseType::BAD_OPERATION:
    case MathResponseType::HARD_OPERATION_SUBMITTED:
        bytes = new uint8_t[1];
        break;
    case MathResponseType::FAST_OPERATION_RESULT:
        bytes = new uint8_t[1 + sizeof(int64_t)];
        int64AsBytes(_result, &bytes[1]);
        break;
    default:
        return nullptr;
    }
    bytes[0] = uint8_t(_type);

    return bytes;
}

MathResponseType MathResponse::type() {
    return _type;
}

int64_t MathResponse::result() {
    return _result;
}

uint8_t MathResponse::size() {
    switch (_type) {
    case MathResponseType::BAD_OPERATION:
    case MathResponseType::HARD_OPERATION_SUBMITTED:
        return 1;
    case MathResponseType::FAST_OPERATION_RESULT:
        return 1 + sizeof(int64_t);
    default:
        return 0;
    }
}

MathResponseType MathResponse::typeOf(uint8_t *bytes) {
    return MathResponseType(bytes[0]);
}

int64_t MathResponse::resultOf(uint8_t *bytes) {
    return bytesAsInt64(&bytes[1]);
}
