#if defined(VEERWOLF)

#include "lib/uartio.h"

#elif defined(EL2SIM)

#include "el2sim/hostio.h"

#else

#include <stdio.h>

#endif

int main() {

    printf("Hello world!\n");

    return 0;
}
