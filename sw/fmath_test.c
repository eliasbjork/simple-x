#include "lib/fmath.h"
#include "lib/uartio.h"

#define TEST(X) (X) ? "TRUE" : "FALSE"

int main() {
    // fabs
    printf("fabs tests:\n");
    printf("fabs(-1.0) == 1.0: %s\n", TEST(fabs(-1.0f)==1.0f));
    printf("fabs(3.14) == 3.14: %s\n", TEST(fabs(3.14f)==3.14f));
    printf("fabs(0.0) == 0.0: %s\n", TEST(fabs(0.0f)==0.0f));
    printf("fabs(-1 * INFINITY) == INFINITY: %s\n", TEST(fabs(-1.0f * INFINITY)==INFINITY));
    printf("isnan(fabs(NAN)): %s\n", TEST(isnan(fabs(NAN))));
    printf("\n");

    // isinf
    printf("isinf tests:\n");
    printf("isinf(INFINITY): %s\n", TEST(isinf(INFINITY)));
    printf("isinf(-1.0 * INFINITY): %s\n", TEST(isinf(-1.0f * INFINITY)));
    printf("isinf(1.0/0.0): %s\n", TEST(isinf(1.0f/0.0f)));
    printf("isinf(-1.0/0.0): %s\n", TEST(isinf(-1.0f/0.0f)));
    printf("!isinf(3.14): %s\n", TEST(!isinf(3.14f)));
    printf("!isinf(NAN): %s\n", TEST(!isinf(NAN)));
    printf("\n");

    // isnan
    printf("isnan tests:\n");
    printf("isnan(NAN): %s\n", TEST(isnan(NAN)));
    printf("isnan(-1.0 * NAN): %s\n", TEST(isnan(-1.0f * NAN)));
    printf("isnan(0.0/0.0): %s\n", TEST(isnan(0.0f/0.0f)));
    printf("!isnan(3.14): %s\n", TEST(!isnan(3.14f)));
    printf("!isnan(INFINITY): %s\n", TEST(!isnan(INFINITY)));
    printf("NAN != NAN: %s\n", TEST(NAN != NAN));
    printf("\n");
}