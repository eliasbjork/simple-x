#include "lib/uartio.h"

int main() {
    char c;
    for (;;) {
        c = (char)getch();
        printf("Recieved: %c\n", c);
    }
}