//
// Created by anton.lamtev on 02.10.2018.
//

#ifndef NETS_BITSUTILS_H
#define NETS_BITSUTILS_H

#include <cstdint>


inline void int64AsBytes(int64_t int64, uint8_t *bytes) {
    for (auto i = 0; i < 8; ++i) {
        bytes[7 - i] = (uint8_t) (int64 >> (i << 3));
    }
}

inline int64_t bytesAsInt64(const uint8_t *bytes) {
    return (((int64_t) bytes[0]) << 56 |
            ((int64_t) bytes[1]) << 48 |
            ((int64_t) bytes[2]) << 40 |
            ((int64_t) bytes[3]) << 32 |
            ((int64_t) bytes[4]) << 24 |
            ((int64_t) bytes[5]) << 16 |
            ((int64_t) bytes[6]) << 8 |
            ((int64_t) bytes[7]));
}

inline void uint64AsBytes(uint64_t uint64, uint8_t *bytes) {
    for (auto i = 0; i < 8; ++i) {
        bytes[7 - i] = (uint8_t) (uint64 >> (i << 3));
    }
}

inline uint64_t bytesAsUint64(const uint8_t *bytes) {
    return (((uint64_t) bytes[0]) << 56 |
            ((uint64_t) bytes[1]) << 48 |
            ((uint64_t) bytes[2]) << 40 |
            ((uint64_t) bytes[3]) << 32 |
            ((uint64_t) bytes[4]) << 24 |
            ((uint64_t) bytes[5]) << 16 |
            ((uint64_t) bytes[6]) << 8 |
            ((uint64_t) bytes[7]));
}

#endif //NETS_BITSUTILS_H
