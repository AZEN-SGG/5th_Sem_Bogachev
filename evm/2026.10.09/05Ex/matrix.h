#ifndef MATRIX_H
#define MATRIX_H

#include <cmath>
#include <cstddef>

#include "block.h"

void init_vec_b (const size_t n, const double *a, double *b);

double get_r1 (const size_t n, const size_t m, const double *a,
               const double *b, const double *x, double *work);

double get_r2 (const size_t n, const double *x);

double get_matrix_norm (const size_t n, const double *a);

#endif
