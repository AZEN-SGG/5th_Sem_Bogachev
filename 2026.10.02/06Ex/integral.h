#ifndef INTEGRAL_H
#define INTEGRAL_H

#include <cfloat>
#include <cmath>

#define MAX_ITER 30

int simpson (double (*f) (double), double a, double b, double eps,
             double *res);

#endif
