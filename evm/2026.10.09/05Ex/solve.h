#ifndef SOLVE_H
#define SOLVE_H

#include <cmath>
#include <cstddef>

#include "cholesky.h"
#include "comp_status.h"
#include "matrix.h"

comp_status t5_solve (const size_t n, const size_t m, double *a, double *b,
                      int *d, double *work);

#endif
