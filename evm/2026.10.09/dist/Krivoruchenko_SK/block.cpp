#include "block.h"

void get_block_column (const size_t n, const double *a,
    const size_t m, const size_t l, double *block, size_t k, size_t t)
{
  for (size_t i = 0, ki = k * n; i < m; ++i, ki += n)
    for (size_t j = t, jmi = i; j < l + t; ++j, jmi += m)
      block[jmi] = a[ki + j];
}

void set_block_column (const size_t n, double *a,
    const size_t m, const size_t l, const double *block, size_t k, size_t t)
{
  for (size_t i = 0, ki = k * n; i < m; ++i, ki += n)
    for (size_t j = t, jmi = i; j < l + t; ++j, jmi += m)
      a[ki + j] = block[jmi];
}

void get_block_row (const size_t n, const double *a,
    const size_t m, const size_t l, double *block, size_t k, size_t t)
{
  for (size_t i = 0, ki = k * n, il = 0; i < m; ++i, ki += n, il += l)
    for (size_t j = t, ilj = il; j < l + t; ++j, ++ilj)
      block[ilj] = a[ki + j];
}

void set_block_row (const size_t n, double *a,
    const size_t m, const size_t l, const double *block, size_t k, size_t t)
{
  for (size_t i = 0, ki = k * n, il = 0; i < m; ++i, ki += n, il += l)
    for (size_t j = t, ilj = il; j < l + t; ++j, ++ilj)
      a[ki + j] = block[ilj];
}
