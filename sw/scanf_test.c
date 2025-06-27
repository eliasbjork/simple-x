#include "lib/uartio.h"

int main() {
    int a, b;
    for (;;) {
        scanf("a=%d, b=%d.", &a, &b);
        printf("Recieved: a=%d, b=%d\n", a, b);
    }
}