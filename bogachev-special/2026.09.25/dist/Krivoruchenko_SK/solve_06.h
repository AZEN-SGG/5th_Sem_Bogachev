#ifndef SOLVE_H
#define SOLVE_H

#include <cfloat>
#include <cmath>
#include <cstdbool>
#include <cstdint>
#include <cstring>

int t6_solve (double (*f) (double), double a, double b, double eps, int m,
              double *x);

#endif
