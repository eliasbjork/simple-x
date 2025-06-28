#include "../lib/mem.h"
#include "../lib/uartio.h"
#include "util.h"


float** make_matrix(int m, int n) {
    float** a;
    int i;

    a = calloc(m, sizeof(float*));

    for (i=0; i<m; i++)
        a[i] = calloc(n, sizeof(float));

    return a;
}


void free_matrix(float** a, int nbr_rows) {
    for (int i=0; i<nbr_rows; i++)
        free(a[i]);
    free(a);
}


void scan_vec(float v[], int len) {
    int i;
    for (i=0; i<len; i++)
        scanf("%lf", &v[i]);
}


void scan_matrix(float** a, int m, int n) {
    int i,j;
    for (i=0; i<m; i++)
    {
        for (j=0; j<n; j++)
        {
            scanf("%lf", &a[i][j]);
        }
    }
}


void print_vec(const float v[], int len) {
    int i;
    printf("\t");
    for (i=0; i<len; i++)
    {
        printf("%10.3lf", v[i]);
    }
    printf("\n");
}


void print_matrix(float** const a, int m, int n) {
    int i,j;
    for (i=0; i<m; i++)
    {
        printf("\t");
        for (j=0; j<n; j++)
        {
            printf("%10.3lf", a[i][j]);
        }
        printf("\n");
    }
}


void print_obj_fun(const float c[], int len) {
    int i;

    printf("max z =\t");

    for (i=0; i<len-1; i++)
        printf("%6.3lf * x%d + ", c[i], i);

    printf("%6.3lf * x%d\n", c[i], i);
}


void print_constraints(float** const a, const float b[], int m, int n) {
    int i,j;
    for (i=0; i<m; i++)
    {
        printf("\t");
        for (j=0; j<n-1; j++)
        {
            printf("%6.3lf * x%d + ", a[i][j], j);
        }
        printf("%6.3lf * x%d\t\u2264 %6.3lf \n", a[i][j], j, b[i]);
    }
}