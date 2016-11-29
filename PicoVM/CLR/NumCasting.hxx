#ifndef __NUMCASTING_HXX__
#define __NUMCASTING_HXX__

#include <cstdint>

inline uint32_t floatToUInt(float value) {
    union {
        float f;
        uint32_t i;
    } u;

    u.f = value;

    return u.i;
}

inline uint64_t doubleToULong(double value) {
    union {
        double d;
        uint64_t l;
    } u;

    u.d = value;

    return u.l;
}

inline float uintToFloat(uint32_t value) {
    union {
        float f;
        uint32_t i;
    } u;
    u.f = value;
    return u.i;
}

inline double ulongToDouble(uint64_t value) {
    union {
        double d;
        uint64_t l;
    } u;

    u.l = value;
    return u.d;
}

#endif
