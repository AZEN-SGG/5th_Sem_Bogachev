#ifndef ARRAY_IO_H
#define ARRAY_IO_H

#include <cstdio>
#include <cstring>

#include "io_status.h"

io_status read_values (double *X, double *Y, const int n, const char *name);
void print_values (const double *X, const double *Y, const int n, const int p);
io_status read_values_and_derivatives (double *X, double *Y, double *D,
                                       const int n, const char *name);
void print_values_and_derivatives (const double *X, const double *Y,
                                   const double *D, const int n, const int p);

#endif
