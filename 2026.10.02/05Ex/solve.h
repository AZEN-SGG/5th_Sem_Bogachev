#ifndef SOLVE_H
#define SOLVE_H

#include <cfloat>
#include <cmath>

#define MAX_ITER 30

double t5_solve (double (*f) (double), double a, double eps, double *res);

#endif
