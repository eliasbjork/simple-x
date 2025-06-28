#ifndef UTIL_H
#define UTIL_H

float** make_matrix(int m, int n);

void free_matrix(float** a, int nbr_rows);

void scan_vec(float v[], int len);

void scan_matrix(float** a, int m, int n);

void print_vec(const float v[], int len);

void print_matrix(float** const a, int m, int n);

void print_obj_fun(const float c[], int len);

void print_constraints(float** const a, const float b[], int m, int n);

#endif // UTIL_H