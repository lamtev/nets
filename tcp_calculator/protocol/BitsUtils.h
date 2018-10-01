//
// Created by anton.lamtev on 02.10.2018.
//

#ifndef NETS_BITSUTILS_H
#define NETS_BITSUTILS_H


#include <cstdint>

inline void int64AsBytes(int64_t _long, uint8_t *bytes) {
    for (auto i = 0; i < 8; ++i) {
        bytes[7 - i] = (unsigned char) (_long >> (i << 3));
    }
}

inline long bytesAsLong(const uint8_t *bytes) {
    return (((int64_t) bytes[0]) << 56 |
            ((int64_t) bytes[1]) << 48 |
            ((int64_t) bytes[2]) << 40 |
            ((int64_t) bytes[3]) << 32 |
            ((int64_t) bytes[4]) << 24 |
            ((int64_t) bytes[5]) << 16 |
            ((int64_t) bytes[6]) << 8 |
            ((int64_t) bytes[7]));
}

#endif //NETS_BITSUTILS_H