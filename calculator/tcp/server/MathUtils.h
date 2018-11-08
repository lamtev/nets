//
// Created by anton.lamtev on 08.10.2018.
//

#ifndef NETS_MATHUTILS_H
#define NETS_MATHUTILS_H

#include <cstdint>

inline uint64_t factorial(uint64_t n) {
    uint64_t res = 1;
    while (n > 0) {
        res *= (n--);
    }

    return res;
}

#endif //NETS_MATHUTILS_H
