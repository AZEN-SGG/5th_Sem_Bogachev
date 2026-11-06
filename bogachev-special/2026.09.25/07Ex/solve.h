#ifndef SOLVE_H
#define SOLVE_H

#include <cfloat>
#include <cmath>

int t7_solve (double (*f) (double), double a, double b, double eps, int m,
              double *x);

#endif
