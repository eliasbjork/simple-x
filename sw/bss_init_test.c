#include "lib/uartio.h"

#define TEST(X) (X) ? "TRUE" : "FALSE"

int glob;
static int stat_glob;

int main() {
    static int stat_loc;

    printf("Global zero-initialized: %s\n", TEST(glob == 0));
    printf("Static global zero-initialized: %s\n", TEST(stat_glob == 0));
    printf("Static local zero-initialized: %s\n", TEST(stat_loc == 0));
}