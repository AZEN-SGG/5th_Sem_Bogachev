#ifndef ARRAY_IO_H
#define ARRAY_IO_H

#include <cstddef>

#include "io_status.h"
#include "init_f.h"

io_status
read_matrix (const size_t n, double *a, const char *filename);

void
print_matrix (size_t n, const double *a, size_t r);

void
init_matrix (const size_t n, double *a, const int s);

void
init_identity_matrix (double *a, int n);

int
read_or_init_matrix (const size_t n, double *a, const char *filename, 
    const int s);

void
print_vector (const size_t n, const double *vec, const size_t r);

#endif
