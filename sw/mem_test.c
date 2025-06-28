#include "lib/mem.h"
#include "lib/uartio.h"


int main() {
    int* ptr;

    size_t p_len = 4;
    size_t p_size = p_len * sizeof(int);
    int* p = malloc(p_size);

    size_t q_len = 5;
    size_t q_size = q_len * sizeof(int);
    int* q = calloc(q_len, sizeof(int));

    if (p == NULL || q == NULL) {
        printf("Failed to malloc!\n");
        return 1;
    }

    printf("Before memset:\np = [ ");
    for (ptr = p; ptr < p + p_len; ++ptr) {
        printf("%d ", *ptr);
    }
    printf("]\nq = [ ");
    for (ptr = q; ptr < q + q_len; ++ptr) {
        printf("%d ", *ptr);
    }
    printf("]\n");

    memset(p, 3, p_size);
    memset(q, 5, q_size);

    printf("After memset:\np = [ ");
    for (ptr = p; ptr < p + p_len; ++ptr) {
        printf("%d ", *ptr);
    }
    printf("]\nq = [ ");
    for (ptr = q; ptr < q + q_len; ++ptr) {
        printf("%d ", *ptr);
    }
    printf("]\n");

    p[0] = 1;
    p[3] = 10;
    q[1] = -2;
    q[2] = -5;
    q[4] = 111;

    printf("After setting some random elements:\np = [ ");
    for (ptr = p; ptr < p + p_len; ++ptr) {
        printf("%d ", *ptr);
    }
    printf("]\nq = [ ");
    for (ptr = q; ptr < q + q_len; ++ptr) {
        printf("%d ", *ptr);
    }
    printf("]\n");

    int* r = malloc(128 * 1024 * 1024);
    if (r == NULL) {
        printf("Tried to allocate outside RAM\n");
    } else {
        free(r);
    }

    free(p);
    free(q);
}