#include <stdint.h>
#include "fmath.h"


typedef union f_to_u32 f_to_u32;
union f_to_u32 {
    float f;
    uint32_t u32;
};


inline float __fabs(float f) {
    f_to_u32 x;
    x.f = f;
    x.u32 &= 0xffffffff >> 1;
    return x.f;
}


inline bool __isnan(float f) {
    // return f != f; // Doesn't work with -ffast-math
    f_to_u32 x;
    x.f = f;
    return (x.u32 & 0x7FFFFFFF) > 0x7F800000;
}


inline bool __isinf(float f) {
    return fabs(f) == INFINITY;
}
