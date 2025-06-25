#include "ext/printf/printf.h"

int main() {
    int i1 = 1;
    int i2 = 2;
    int i3 = i1+i2;
    float f1 = 3.14;
    float f2 = 2.71;
    float f3 = f1+f2;

    for (;;) {
        printf("1 + 2 = %d\n", i3);
        printf("3.14 + 2.71 = %f\n", f3);
    }
}
