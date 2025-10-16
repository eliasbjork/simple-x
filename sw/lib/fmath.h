#ifndef FMATH_H
#define FMATH_H

#include <float.h>
#include <stdbool.h>

#define INFINITY __builtin_inff()
#define NAN __builtin_nanf("")

float fabs(float);
int isinf(float);
int isnan(float);

#endif // FMATH_H