#include "hostio.h"

#define GETCH_REQ_BYTE 0xf0

extern volatile char tohost;


void _putchar(char c) {
    tohost = c;
}


int getch() {
    char read_val;

    tohost = GETCH_REQ_BYTE; // request next input from el2sim tp_top

    do {
        read_val = tohost;
    } while (read_val == GETCH_REQ_BYTE);

    return (int)read_val;
}
