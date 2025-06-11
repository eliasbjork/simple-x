#include <stdio.h>
#include <stdlib.h>
#include "util.h"

#define double float

double** make_matrix(int m, int n) {
    double** a;
    int i;

    a = calloc(m, sizeof(double*));

    for (i=0; i<m; i++)
        a[i] = calloc(n, sizeof(double));

    return a;
}


void free_matrix(double** a, int nbr_rows) {
    for (int i=0; i<nbr_rows; i++)
        free(a[i]);
    free(a);
}


void scan_vec(double v[], int len) {
    int i;
    for (i=0; i<len; i++)
        // scanf("%lf", &v[i]);
        scanf("%f", &v[i]);
}


void scan_matrix(double** a, int m, int n) {
    int i,j;
    for (i=0; i<m; i++)
    {
        for (j=0; j<n; j++)
        {
            // scanf("%lf", &a[i][j]);
            scanf("%f", &a[i][j]);
        }
    }
}


void print_vec(const double v[], int len) {
    int i;
    printf("\t");
    for (i=0; i<len; i++)
    {
        // printf("%10.3lf", v[i]);
        printf("%10.3f", v[i]);
    }
    printf("\n");
}


void print_matrix(double** const a, int m, int n) {
    int i,j;
    for (i=0; i<m; i++)
    {
        printf("\t");
        for (j=0; j<n; j++)
        {
            // printf("%10.3lf", a[i][j]);
            printf("%10.3f", a[i][j]);
        }
        printf("\n");
    }
}


void print_obj_fun(const double c[], int len) {
    int i;

    printf("max z =\t");

    for (i=0; i<len-1; i++)
        // printf("%6.3lf * x%d + ", c[i], i);
        printf("%6.3f * x%d + ", c[i], i);

    // printf("%6.3lf * x%d\n", c[i], i);
    printf("%6.3f * x%d\n", c[i], i);
}


void print_constraints(const double** a, const double b[], int m, int n) {
    int i,j;
    for (i=0; i<m; i++)
    {
        printf("\t");
        for (j=0; j<n-1; j++)
        {
            // printf("%6.3lf * x%d + ", a[i][j], j);
            printf("%6.3f * x%d + ", a[i][j], j);
        }
        // printf("%6.3lf * x%d\t\u2264 %6.3lf \n", a[i][j], j, b[i]);
        printf("%6.3f * x%d\t\u2264 %6.3lf \n", a[i][j], j, b[i]);
    }
}