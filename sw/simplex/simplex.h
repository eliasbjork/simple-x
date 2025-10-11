#ifndef SIMPLEX_H
#define SIMPLEX_H

typedef struct simplex_t simplex_t;

float simplex(int m, int n, float** a, float* b, float* c, float* x, float y);

float xsimplex(int m, int n, float** a, float* b, float* c, float* x, float y, int* var, int h);

int initial(simplex_t* s, int m, int n, float** a, float* b, float* c, float* x, float y, int* var);

int init(simplex_t* s, int m, int n, float** a, float* b, float* c, float* x, float y, int* var);

void prepare(simplex_t* s, int k);

void pivot(simplex_t* s, int row, int col);

int select_nonbasic(simplex_t s);

#endif // SIMPLEX_H