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
    int m;       // num constraints
    int n;       // num decision vars
    int* var_at; // n+m+1 array mapping indices to original indices
    float** a;   // m*(n+1) matrix
    float* b;    // m bounds
    float* x;    // n+m decision + slack vars
    float* c;    // n constants
    float y;     // objective function max value
    int prev_q;  // prev pivot col
};


float simplex(int m, int n, float** a, float* b, float* c, float* x, float y) {
    return xsimplex(m, n, a, b, c, x, y, NULL, n+1, 0);
}


float xsimplex(int m, int n, float** a, float* b, float* c, float* x, float y, int* var_at, int prev_q, int h) {
    simplex_t s;
    int i, row, col;

    if (!(initial(&s, m, n, a, b, c, x, y, var_at, prev_q))) {
        free(s.var_at);
        return NAN;
    }

    while (col = select_nonbasic(s), col >= 0) {
        row = -1;
        
        // find which constraint is tightest for nonbasic with index col
        for (i = 0; i < m; i++) {
            if (a[i][col] > EPSILON && (row < 0 || b[i]/a[i][col] < b[row]/a[row][col])) {
                row = i;
            }
        }

        // if none of the constraints limits nonbasic with index col
        if (row < 0) {
            free(s.var_at);
            return INFINITY;
        }

        pivot(&s, row, col);
    }

    if (h == 0) {
        for (i = 0; i < n; i++) {
            if (s.var_at[i] < n) {
                x[s.var_at[i]] = 0;
            }
        }
        for (i = 0; i < m; i++) {
            if (s.var_at[n+i] < n) {
                x[s.var_at[n+i]] = s.b[i];
            }
        }
        free(s.var_at);
    } else {
        for (i = 0; i < n; i++)
            x[i] = 0;
        for (i = n; i < n+m; i++)
            x[i] = s.b[i-n];
    }
    return s.y;
}


int initial(simplex_t* s, int m, int n, float** a, float* b, float* c, float* x, float y, int* var_at, int prev_q) {
    int i, j, k;
    float w;

    k = init(s, m, n, a, b, c, x, y, var_at, prev_q);

    if (b[k] >= 0)
        return 1; // feasible solution

    prepare(s, k);
    n = s->n;

    s->y = xsimplex(m, n, s->a, s->b, s->c, s->x, 0, s->var_at, s->prev_q, 1);

    for (i = 0; i < m+n; i++)
        if (s->var_at[i] == m+n-1) {
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
        k = s->var_at[i];
        s->var_at[i] = s->var_at[n-1];
        s->var_at[n-1] = k;

        if (i == s->prev_q) {
            s->prev_q = n-1;
        } else if (n-1 == s->prev_q) {
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
        s->var_at[k] = s->var_at[k+1];

    n = s->n-1;
    s->n = s->n-1;
    s->prev_q = n;

    float* t = calloc(n, sizeof(float));

    for (k = 0; k < n; k++) {
        for (j = 0; j < n; j++)
            if (k == s->var_at[j]) {
                // x_k is nonbasic, add c_k
                t[j] = t[j] + s->c[k];
                goto next_k;
            }

        // x_k is basic
        for (j = 0; j < m; j++)
            if (s->var_at[n+j] == k)
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


int init(simplex_t* s, int m, int n, float** a, float* b, float* c, float* x, float y, int* var_at, int prev_q) {
    int i, k;

    s->m = m;
    s->n = n;
    s->a = a;
    s->b = b;
    s->x = x;
    s->c = c;
    s->y = y;
    s->prev_q = prev_q;

    if (var_at == NULL) {
        var_at = calloc(m+n+1, sizeof(int));

        for (i = 0; i < m+n+1; i++)
            var_at[i] = i;
    }
    s->var_at = var_at;

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
        s->var_at[i] = s->var_at[i-1];

    s->var_at[n] = m+n;

    n = n+1;

    for (i = 0; i < m; i++)
        s->a[i][n-1] = -1;

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
    int i, j, t;
    float a_pq_inv, a_iq;

    // p and q are the indices of the pivot row and column, respectively

    // prepare extra column
    for (i = 0; i < m; i++)
        a[i][s->prev_q] = 0;

    a[p][s->prev_q] = 1;

    // calculate reciprocal pivot element
    a_pq_inv = 1/a[p][q];

    // update the objective function, constants and bounds
    s->y = s->y + c[q]*b[p]*a_pq_inv;

    for (i = 0; i < n; i++)
        if (i != q)
            c[i] = c[i] - c[q]*a[p][i]*a_pq_inv;

    c[q] = -c[q]*a_pq_inv;

    for (i = 0; i < m; i++)
        if (i != p)
            b[i] = b[i] - a[i][q]*b[p]*a_pq_inv;

    b[p] = b[p]*a_pq_inv;

    // update pivot row
    for (i = 0; i < n+1; i++)
        a[p][i] = a[p][i]*a_pq_inv;

    // update all other rows
    for (i = 0; i < m; i++)
        if (i != p) {
            a_iq = a[i][q];
            for (j = 0; j < n+1; j++)
                if (j != q)
                    a[i][j] = a[i][j] - a_iq*a[p][j];
        }

    // copy extra column into pivot column
    for (i = 0; i < m; i++)
        a[i][q] = a[i][s->prev_q];

    // exchange indices
    t = s->var_at[q];
    s->var_at[q] = s->var_at[n+p];
    s->var_at[n+p] = t;
}


int select_nonbasic(simplex_t s) {
    int i, max;

    // Dantzig's rule i.e. always pick the steepest direction
    // (i corresponding to largest c[i])

    for (max = 0, i = 1; i < s.n; i++) {
        if (s.c[max] < s.c[i]) {
            max = i;
        }
    }

    if (s.c[max] > EPSILON) {
        return max;
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
