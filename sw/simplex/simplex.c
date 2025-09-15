#ifndef VEERWOLF

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#else

#include "../lib/fmath.h"
#include "../lib/mem.h"
#include "../lib/uartio.h"

#endif

#include "simplex.h"
#include "util.h"


static const float EPSILON = 1e-5;


struct simplex_t {
    int m;
    int n;
    int* var;    // n+m+1
    float** a;   // m x n+1
    float* b;    // m
    float* x;    // n+m
    float* c;    // n
    float y;
    int prev_p;
    int prev_q;
};


float simplex(int m, int n, float** a, float* b, float* c, float* x, float y) {
    return xsimplex(m, n, a, b, c, x, y, NULL,0, n+1, 0);
}


float xsimplex(int m, int n, float** a, float* b, float* c, float* x, float y, int* var, int prev_p, int prev_q, int h) {
    simplex_t s;
    int i, row, col;

    if (!(initial(&s, m, n, a, b, c, x, y, var, prev_p, prev_q))) {
        free(s.var);
        return NAN;
    }

    while (col = select_nonbasic(s), col >= 0) {
        row = -1;
        
        // find which constraint is tightest for non-basic with index col
        for (i = 0; i < m; i++) {
            if (a[i][col] > EPSILON && (row < 0 || b[i]/a[i][col] < b[row]/a[row][col])) {
                row = i;
            }
        }

        // if non of the constraints limits non-basic with index col
        if (row < 0) {
            free(s.var);
            return INFINITY;
        }
        printf("h = %d\n", h);
        pivot(&s, row, col);
    }

    if (h == 0) {
        for (i = 0; i < n; i++) {
            if (s.var[i] < n) {
                x[s.var[i]] = 0;
            }
        }
        for (i = 0; i < m; i++) {
            if (s.var[n+i] < n) {
                x[s.var[n+i]] = s.b[i];
            }
        }
        free(s.var);
    } else {
        for (i = 0; i < n; i++)
            x[i] = 0;
        for (i = n; i < n+m; i++)
            x[i] = s.b[i-n];
    }
    return s.y;
}


int initial(simplex_t* s, int m, int n, float** a, float* b, float* c, float* x, float y, int* var, int prev_p, int prev_q) {
    int i,j,k;
    float w;

    k = init(s, m, n, a, b, c, x, y, var, prev_p, prev_q);

    if (b[k] >= 0)
        return 1; // feasible solution

    prepare(s, k);
    n = s->n;

    s->y = xsimplex(m, n, s->a, s->b, s->c, s->x, 0, s->var, s->prev_p, s->prev_q, 1);

    for (i = 0; i < m+n; i++)
        if (s->var[i] == m+n-1) {
            if (fabs(s->x[i]) > EPSILON) {
                free(s->x);
                free(s->c);
                return 0; // infeasible
            } else {
                break;
            }
        }

    if (i >= n) {
        for (j = 0, k = 0; k < n; k++)
            if (fabs(s->a[i-n][k]) > fabs(s->a[i-n][j]))
                j = k;

        pivot(s, i-n, j);
        i = j;
    }

    if (i < n-1) {
        k = s->var[i];
        s->var[i] = s->var[n-1];
        s->var[n-1] = k;

        if(i == s->prev_q){
            s->prev_q = n-1;
        } else if(n-1 == s->prev_q){
            s->prev_q = i;
        }

        for (k = 0; k < m; k++) {
            w = s->a[k][n-1];
            s->a[k][n-1] = s->a[k][i];
            s->a[k][i] = w;
        }
    }

    free(s->c);
    s->c = c;
    s->y = y;

    for (k = n-1; k < n+m-1; k++)
        s->var[k] = s->var[k+1];

    n = s->n-1;
    s->n = s->n-1;
    s->prev_q = n;

    float* t = calloc(n, sizeof(float));

    for (k = 0; k < n; k++) {
        for (j = 0; j < n; j++)
            if (k == s->var[j]) {
                // x_k is nonbasic, add c_k
                t[j] = t[j] + s->c[k];
                goto next_k;
            }

        // x_k is basic
        for (j = 0; j < m; j++)
            if (s->var[n+j] == k)
                // x_k is at row j
                break;

        s->y = s->y + s->c[k]*s->b[j];

        for (i = 0; i < n; i++)
            t[i] = t[i] - s->c[k]*s->a[j][i];

        next_k:;
    }

    for (i = 0; i < n; i++)
        s->c[i] = t[i];

    free(t);
    free(s->x);

    return 1;
}


int init(simplex_t* s, int m, int n, float** a, float* b, float* c, float* x, float y, int* var, int prev_p, int prev_q) {
    int i, k;

    s->m = m;
    s->n = n;
    // s->var = var;
    s->a = a;
    s->b = b;
    s->x = x;
    s->c = c;
    s->y = y;
    s->prev_p = prev_p;
    s->prev_q = prev_q;

    if (var == NULL) {
        printf("Initializing variable mapping\n");
        var = calloc(m+n+1, sizeof(int));

        for (i = 0; i < m+n+1; i++)
            var[i] = i;

    }
    s->var = var;


    for (k = 0, i = 1; i < m; i++) {
        if (b[i] < b[k])
            k = i;
    }

    return k;
}


void prepare(simplex_t* s, int k) {
    int m = s->m;
    int n = s->n;
    int i;

    for (i = m+n; i > n; i--)
    // for (i = m+n; i > n+1; i--)
        s->var[i] = s->var[i-1];

    s->var[n] = m+n;
    // s->var[n+1] = m+n+1;

    n = n+1;

    for (i = 0; i < m; i++)
        s->a[i][n-1] = -1;
        // s->a[i][n] = -1;

    s->x = calloc(m+n, sizeof(float));
    s->c = calloc(n, sizeof(float));

    s->c[n-1] = -1;
    s->n = n;


    pivot(s, k, n-1);
}


void pivot(simplex_t* s, int p, int q) {
    float** a = s->a;
    float* b = s->b;
    float* c = s->c;
    int m = s->m;
    int n = s->n;
    int i,j,t;
    float a_pq_inv, a_iq;

    // p and q are the indices of the pivot row and column, respectively
    for (i = 0; i < m+n; i++)
        printf("%d ", s->var[i]);
    printf("\n");
    printf("p = %d, q = %d\n", p, q);

    printf("prev_p = %d, prev_q = %d\n", s->prev_p, s->prev_q);

    // t = s->var[s->prev_q];
    // s->var[s->prev_q] = s->var[n+p];
    // s->var[n+p] = t;

    for (i = 0; i < m+n; i++)
        printf("%d ", s->var[i]);
    printf("\n");

    // setup extra column
    for (int i = 0; i < m; i++)
    {
        a[i][s->prev_q] = 0;
    }
    
    a[p][s->prev_q] = 1;

    print_matrix(a, m, n+1);

    a_pq_inv = 1/a[p][q];
    int q_lt_prev_q = (q > s->prev_q);

    s->y = s->y + c[q-q_lt_prev_q]*b[p]*a_pq_inv;

    int past_temp;
    int index;
    for (i = 0, past_temp = 0; i < n; i++) {
        if (i == s->prev_q)
            past_temp = 1;

        index = i - past_temp;
        if (i != q && i != s->prev_q)
            c[index] = c[index] - c[q-q_lt_prev_q]*a[p][i]*a_pq_inv;

    }

    c[q-q_lt_prev_q] = -c[q-q_lt_prev_q]*a_pq_inv;

    for (i = 0; i < m; i++)
        if (i != p)
            b[i] = b[i] - a[i][q]*b[p]*a_pq_inv;

    b[p] = b[p]*a_pq_inv;

    for (i = 0; i < n+1; i++)
        a[p][i] = a[p][i]*a_pq_inv;

    for (i = 0; i < m; i++)
        if (i != p) {
            a_iq = a[i][q];
            for (j = 0; j < n+1; j++)
                if (j != q)
                    a[i][j] = a[i][j] - a_iq*a[p][j];
        }
    
    for (i = 0; i < m; i++){
        a[i][q] = a[i][s->prev_q];
    }
    //testar om det fungerar att flytta skiten alltid till n-1
    // float temp;
    // for (int i = 0; i < m; i++)
    //     {
    //         temp = a[i][s->prev_q];
    //         a[i][s->prev_q] = a[i][q];
    //         a[i][q] = temp;
    //     }
    t = s->var[q];
    s->var[q] = s->var[n+p];
    s->var[n+p] = t;
    //s->prev_q = q;
    printf("After pivoting:\n");
    print_matrix(a, m, n+1);
        printf("\n");

    for (i = 0; i < m+n; i++)
        printf("%d ", s->var[i]);
    printf("\n\n");
}


int select_nonbasic(simplex_t s) {
    int i, max;

    // Dantzig's rule i.e. always pick the steepest direction
    // (i corresponding to largest c[i])
    print_vec(s.c, s.n);
    for (max = 0, i = 1; i < s.n; i++) {
        if (s.c[max] < s.c[i]) {
            max = i;
        }
    }
    printf("select_nonbasic: max = %d, c[max] = %f, prev_q = %d\n", max, s.c[max], s.prev_q);
    if (s.c[max] > EPSILON) {
        if (max < s.prev_q) {
            return max;
        }else
        {
            return max + 1;
        }
        
    }

    return -1;
}


int main() {
    int m;
    int n;

#ifdef VEERWOLF
    for (;;) {
#endif
        scanf("%d %d", &m, &n);

        // printf("m = %d ; n = %d\n", m, n);

        float** a = make_matrix(m, n+2);
        float b[m];
        float c[n];

        float x[n+1];
        float y = 0;

        scan_vec(c, n);
        scan_matrix(a, m, n);
        scan_vec(b, m);


        float sol = simplex(m, n, a, b, c, x, y);

        if (!isnan(sol) && !isinf(sol)) {
            printf("z = %lf\n", sol);
        } else if (isinf(sol)) {
            // the problem is unbounded 
            printf("z = inf\n");
        } else {
            // the problem is infeasible
            printf("z = nan\n");
        }

        free_matrix(a, m);

#ifdef VEERWOLF
        _reset_heap();
    }
#endif

    return 0;
}
