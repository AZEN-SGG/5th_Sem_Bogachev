#ifndef SOLVE_H
#define SOLVE_H

#include <cfloat>
#include <cmath>

#define MAX_ITER 30

int t4_solve (double (*f) (double), double a, double b, double eps,
              double *res);

#endif
