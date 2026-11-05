#ifndef SOLVE_H
#define SOLVE_H

#include <cfloat>
#include <cmath>
#include <cstdbool>
#include <cstdint>
#include <cstring>

int t5_solve (double (*f) (double), double (*d) (double), double x_0,
              double eps, int m, double *x);

#endif
