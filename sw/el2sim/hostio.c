#include "hostio.h"

extern volatile char tohost;

void _putchar(char c) {
    tohost = c;
}