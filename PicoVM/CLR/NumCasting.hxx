#ifndef __NUMCASTING_HXX__
#define __NUMCASTING_HXX__

#include <cstdint>

inline int32_t floatToInt(float value) {
    union {
        float f;
        int32_t i;
    } u;

    u.f = value;

    return u.i;
}

inline int64_t doubleToLong(double value) {
    union {
        double d;
        int64_t l;
    } u;

    u.d = value;

    return u.l;
}

inline float intToFloat(int32_t value) {
    union {
        float f;
        int32_t i;
    } u;
    u.f = value;
    return u.i;
}

inline double longToDouble(int64_t value) {
    union {
        double d;
        int64_t l;
    } u;

    u.l = value;
    return u.d;
}

#endif
