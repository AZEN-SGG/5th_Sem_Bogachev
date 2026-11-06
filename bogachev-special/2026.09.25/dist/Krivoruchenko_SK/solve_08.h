#ifndef SOLVE_H
#define SOLVE_H

#include <cfloat>
#include <cmath>

int t8_solve (double (*f) (double), double x_0, double eps, int m,
              double *x);

#endif
