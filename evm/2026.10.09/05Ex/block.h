#ifndef BLOCK_H
#define BLOCK_H

#include <cstddef>

void get_block_column (const size_t n, const double *a,
    const size_t m, const size_t l, double *block, size_t k, size_t t);

void set_block_column (const size_t n, double *a,
    const size_t m, const size_t l, const double *block, size_t k, size_t t);

void get_block_row (const size_t n, const double *a,
    const size_t m, const size_t l, double *block, size_t k, size_t t);

void set_block_row (const size_t n, double *a,
    const size_t m, const size_t l, const double *block, size_t k, size_t t);

#endif
