#ifndef SIMPLEX_H
#define SIMPLEX_H

typedef struct simplex_t simplex_t;

double simplex(int m, int n, double** a, double* b, double* c, double* x, double y);

double xsimplex(int m, int n, double** a, double* b, double* c, double* x, double y, int* var, int h);

int initial(simplex_t* s, int m, int n, double** a, double* b, double* c, double* x, double y, int* var);

int init(simplex_t* s, int m, int n, double** a, double* b, double* c, double* x, double y, int* var);

void prepare(simplex_t* s, int k);

void pivot(simplex_t* s, int row, int col);

int select_nonbasic(simplex_t s);

#endif // SIMPLEX_H