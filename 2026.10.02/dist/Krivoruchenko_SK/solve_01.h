#ifndef SOLVE_H
#define SOLVE_H

#include <cfloat>
#include <cmath>

#define NUM_FPE 1e-300

double t1_solve (double (*f) (double), double a, double b, int n);

#endif
