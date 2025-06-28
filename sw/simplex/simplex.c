#include "../lib/fmath.h"
#include "../lib/mem.h"
#include "../lib/uartio.h"
#include "simplex.h"
#include "util.h"


static const float eps = 1e-6;


struct simplex_t {
    int m;
    int n;
    int* var;    // n+m+1
    float** a;   // m x n+1
    float* b;    // m
    float* x;    // n+m
    float* c;    // n
    float y;
};


float simplex(int m, int n, float** a, float* b, float* c, float* x, float y) {
    return xsimplex(m, n, a, b, c, x, y, NULL, 0);
}


float xsimplex(int m, int n, float** a, float* b, float* c, float* x, float y, int* var, int h) {
    simplex_t s;
    int i, row, col;

    if (!(initial(&s, m, n, a, b, c, x, y, var))) {
        free(s.var);
        return NAN;
    }

    while (col = select_nonbasic(s), col >= 0) {
        row = -1;

        // find which constraint is tightest for non-basic with index col
        for (i = 0; i < m; i++) {
            if (a[i][col] > eps && (row < 0 || b[i]/a[i][col] < b[row]/a[row][col])) {
                row = i;
            }
        }

        // if non of the constraints limits non-basic with index col
        if (row < 0) {
            free(s.var);
            return INFINITY;
        }

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


int initial(simplex_t* s, int m, int n, float** a, float* b, float* c, float* x, float y, int* var) {
    int i,j,k;
    float w;

    k = init(s, m, n, a, b, c, x, y, var);

    if (b[k] >= 0)
        return 1; // feasible solution

    prepare(s, k);
    n = s->n;

    s->y = xsimplex(m, n, s->a, s->b, s->c, s->x, 0, s->var, 1);

    for (i = 0; i < m+n; i++)
        if (s->var[i] == m+n-1) {
            if (fabs(s->x[i]) > eps) {
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


int init(simplex_t* s, int m, int n, float** a, float* b, float* c, float* x, float y, int* var) {
    int i, k;

    s->m = m;
    s->n = n;
    // s->var = var;
    s->a = a;
    s->b = b;
    s->x = x;
    s->c = c;
    s->y = y;

    if (var == NULL) {
        var = calloc(m+n+1, sizeof(int)); // borde man checka så det inte blir NULL igen?

        for (i = 0; i < m+n; i++)
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
        s->var[i] = s->var[i-1];

    s->var[n] = m+n;

    n = n+1;

    for (i = 0; i < m; i++)
        s->a[i][n-1] = -1;

    s->x = calloc(m+n, sizeof(float));
    s->c = calloc(n, sizeof(float));

    s->c[n-1] = -1;
    s->n = n;

    pivot(s, k, n-1);
}


void pivot(simplex_t* s, int row, int col) {
    float** a = s->a;
    float* b = s->b;
    float* c = s->c;
    int m = s->m;
    int n = s->n;
    int i,j,t;

    t = s->var[col];
    s->var[col] = s->var[n+row];
    s->var[n+row] = t;

    s->y = s->y + c[col]*b[row]/a[row][col];

    for (i = 0; i < n; i++)
        if (i != col)
            c[i] = c[i] - c[col]*a[row][i]/a[row][col];

    c[col] = -c[col]/a[row][col];

    for (i = 0; i < m; i++)
        if (i != row)
            b[i] = b[i] - a[i][col]*b[row]/a[row][col];

    for (i = 0; i < m; i++)
        if (i != row)
            for (j = 0; j < n; j++)
                if (j != col)
                    a[i][j] = a[i][j] - a[i][col]*a[row][j]/a[row][col];

    for ( i = 0; i < m; i++)
        if (i != row)
            a[i][col] = -a[i][col]/a[row][col];

    for (i = 0; i < n; i++)
        if (i != col)
            a[row][i] = a[row][i]/a[row][col];

    b[row] = b[row]/a[row][col];
    a[row][col] = 1/a[row][col];
}


int select_nonbasic(simplex_t s) {
    int i;
    for (i=0; i<s.n; i++)       // använd dantzig's regel dvs välj i med störst c[i] först. jämför skillnad i tidsåtgång
        if (s.c[i] > eps)
            return i;
    return -1;
}


/* Use Dantzig's rule to select nonbasic i.e. always pick the steepest direction (i corresponding to largest c[i]) */
int select_nonbasic_dantzig_rule(simplex_t s) {
    int i, max;

    for (max = 0, i=1; i<s.n; i++)
        if (s.c[max] < s.c[i])
            max = i;

    if (s.c[max] > eps)
        return max;
    return -1;
}


int main() {
    int m;
    int n;

    scanf("%d %d", &m, &n);

    // printf("m = %d ; n = %d\n", m, n);

    float** a = make_matrix(m, n+1);
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

    return 0;
}
