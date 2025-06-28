#ifndef FMATH_H
#define FMATH_H

#include <float.h>
#include <stdbool.h>

#define INFINITY __builtin_inff()
#define NAN __builtin_nanf("")

#define fabs __fabs
#define isinf __isinf
#define isnan __isnan

float __fabs(float);
bool __isinf(float);
bool __isnan(float);

#endif // FMATH_H