#include "matrix.h"


void
init_vec_b (const size_t n, const double *a, double *b)
{
  for (size_t i = 0; i < n; ++i)
    {
      double sum = 0;

      for (size_t k = 0; k < n; k += 2)
        sum += a[i * n + k];

      b[i] = sum;
    }
}

double
get_r1 (const size_t n, const size_t m, const double *a, const double *b,
        const double *x, double *work)
{
  const size_t l = n % m;
  const size_t end = n - l;
  const size_t m3 = m % 3, l3 = l % 3;
  double up = 0, down = 0;
  double *block = work, *v = work + m * m;

  for (size_t k = 0; k < end; k += m)
    {
      for (size_t i = 0; i < m; ++i)
        {
          v[i] = b[k + i];
          down += fabs (v[i]);
        }

      for (size_t j = 0; j < end; j += m)
        {
          get_block_row (n, a, m, m, block, k, j);
          const double *q = x + j;

          for (size_t i = 0, im = 0; i < m; ++i, im += m)
            {
              const double *p = block + im;
              double s0 = 0, s1 = 0, s2 = 0;
              size_t t = 0;

              for (; t < m3; ++t)
                s0 += p[t] * q[t];

              for (; t < m; t += 3)
                {
                  s0 += p[t] * q[t];
                  s1 += p[t + 1] * q[t + 1];
                  s2 += p[t + 2] * q[t + 2];
                }

              v[i] -= s0 + s1 + s2;
            }
        }

      if (l)
        {
          get_block_row (n, a, m, l, block, k, end);
          const double *q = x + end;

          for (size_t i = 0, il = 0; i < m; ++i, il += l)
            {
              const double *p = block + il;
              double s0 = 0, s1 = 0, s2 = 0;
              size_t t = 0;

              for (; t < l3; ++t)
                s0 += p[t] * q[t];

              for (; t < l; t += 3)
                {
                  s0 += p[t] * q[t];
                  s1 += p[t + 1] * q[t + 1];
                  s2 += p[t + 2] * q[t + 2];
                }

              v[i] -= s0 + s1 + s2;
            }
        }

      for (size_t i = 0; i < m; ++i)
        up += fabs (v[i]);
    }

  if (l)
    {
      for (size_t i = 0; i < l; ++i)
        {
          v[i] = b[end + i];
          down += fabs (v[i]);
        }

      for (size_t j = 0; j < end; j += m)
        {
          get_block_row (n, a, l, m, block, end, j);
          const double *q = x + j;

          for (size_t i = 0, im = 0; i < l; ++i, im += m)
            {
              const double *p = block + im;
              double s0 = 0, s1 = 0, s2 = 0;
              size_t t = 0;

              for (; t < m3; ++t)
                s0 += p[t] * q[t];

              for (; t < m; t += 3)
                {
                  s0 += p[t] * q[t];
                  s1 += p[t + 1] * q[t + 1];
                  s2 += p[t + 2] * q[t + 2];
                }

              v[i] -= s0 + s1 + s2;
            }
        }

      get_block_row (n, a, l, l, block, end, end);
      const double *q = x + end;

      for (size_t i = 0, il = 0; i < l; ++i, il += l)
        {
          const double *p = block + il;
          double s0 = 0, s1 = 0, s2 = 0;
          size_t t = 0;

          for (; t < l3; ++t)
            s0 += p[t] * q[t];

          for (; t < l; t += 3)
            {
              s0 += p[t] * q[t];
              s1 += p[t + 1] * q[t + 1];
              s2 += p[t + 2] * q[t + 2];
            }

          v[i] -= s0 + s1 + s2;
          up += fabs (v[i]);
        }
    }

  return up / down;
}

double
get_r2 (const size_t n, const double *x)
{
  double up = 0, down = 0;

  for (size_t i = 0; i < n; ++i)
    {
      int temp = (i + 1) % 2;
      up += std::abs (x[i] - temp);
      down += temp;
    }

  return up / down;
}

double
get_matrix_norm (const size_t n, const double *a)
{
  double norm = 0;
  for (size_t in = 0; in < n * n; in += n)
    {
      double sum = 0;
      for (size_t j = 0; j < n; ++j)
        sum += std::abs (a[in + j]);

      if (sum > norm)
        norm = sum;
    }

  return norm;
}
