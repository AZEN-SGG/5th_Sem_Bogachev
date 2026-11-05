#include "solve_08.h"
#include <cstdio>

int
t8_solve (const double *arr_a, const int n, const double *arr_b, const int m,
          double *arr_c)
{
  int i, j, k, rep = 0;

  for (i = j = k = 0; i < n && j < m; k++)
    {
      if (arr_a[i] - arr_b[j] > EPS)
        arr_c[k] = arr_b[j++];
      else if (arr_b[j] - arr_a[i] > EPS)
        arr_c[k] = arr_a[i++];
      else
        {
          i++;
          rep++;
          k--;
        }
    }

  for (; i < n; ++i, ++k)
    arr_c[k] = arr_a[i];

  for (; j < m; ++j, ++k)
    arr_c[k] = arr_b[j];

  return n + m - rep;
}
