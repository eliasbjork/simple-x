#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdlib.h>

#define double float

double** make_matrix(int m, int n);

void free_matrix(double** a, int nbr_rows);

void scan_vec(double v[], int len);

void scan_matrix(double** a, int m, int n);

void print_vec(const double v[], int len);

void print_matrix(double** const a, int m, int n);

void print_obj_fun(const double c[], int len);

void print_constraints(const double** a, const double b[], int m, int n);

#endif // UTIL_H