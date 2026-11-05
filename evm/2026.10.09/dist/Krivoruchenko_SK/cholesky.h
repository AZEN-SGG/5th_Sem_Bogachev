#ifndef CHOLESKY_H
#define CHOLESKY_H

#include <cmath>
#include <cstddef>

#include "block.h"
#include "comp_status.h"

#define DBL_EPS 1e-15
#define SQRT_DBL_EPS 3.1622776601683793e-8

comp_status cholesky_method (const size_t n, const size_t m, double *a,
                             double *b, int *d, double *work,
                             const double norm);

#endif
