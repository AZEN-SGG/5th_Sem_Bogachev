#include "cholesky.h"

static comp_status
cholesky_decomposition (const size_t m, double *r, int *d, double *x,
                        const double norm)
{
  for (size_t i = 0, im = 0; i < m; ++i, im += m)
    {
      double denominator, el = 0;

      /* r saved columns as rows.  */
      for (size_t k = 0; k < i; ++k)
        el += r[im + k] * r[im + k] * d[k];

      el = r[im + i] - el;
      d[i] = (el > 0) - (el < 0);
      denominator = std::sqrt (std::abs (el));
      r[im + i] = denominator;

      if (std::abs (denominator) < SQRT_DBL_EPS * norm)
        return comp_status::ERROR_DBZ;

      double zi = x[i] / denominator;
      x[i] = zi;

      denominator = 1 / (denominator * d[i]);
      for (size_t j = i + 1, jm = (i + 1) * m; j < m; ++j, jm += m)
        {
          el = 0;
          for (size_t k = 0; k < i; ++k)
            el += r[im + k] * r[jm + k] * d[k];

          el = (r[im + j] - el) * denominator;
          r[im + j] = el;
          r[jm + i] = el;
          x[j] -= el * zi;
        }
    }

  return comp_status::SUCCESS;
}

/* Block[j * m + i] = R_ij.  */
static comp_status
cholesky_block_row (const size_t m, const size_t l, const double *r,
                    const int *d, double *block, const double *z, double *b,
                    const double norm)
{
  const size_t l3 = l % 3;

  for (size_t i = 0, im = 0; i < m; ++i, im += m)
    {
      const double *p = r + im;

      if (std::abs (p[i]) < SQRT_DBL_EPS * norm)
        return comp_status::ERROR_DBZ;

      double denominator = 1 / (p[i] * d[i]);
      double zi = z[i];
      size_t j = 0;

      for (; j < l3; ++j)
        {
          double *q = block + j * m;
          double el = 0;
          for (size_t k = 0; k < i; ++k)
            el += p[k] * q[k] * d[k];

          el = (q[i] - el) * denominator;
          q[i] = el;
          b[j] -= el * zi;
        }

      for (; j < l; j += 3)
        {
          double *q0 = block + j * m;
          double *q1 = q0 + m;
          double *q2 = q1 + m;
          double s0 = 0, s1 = 0, s2 = 0;

          for (size_t k = 0; k < i; ++k)
            {
              double el = p[k] * d[k];
              s0 += el * q0[k];
              s1 += el * q1[k];
              s2 += el * q2[k];
            }

          s0 = (q0[i] - s0) * denominator;
          s1 = (q1[i] - s1) * denominator;
          s2 = (q2[i] - s2) * denominator;
          q0[i] = s0;
          q1[i] = s1;
          q2[i] = s2;
          b[j] -= s0 * zi;
          b[j + 1] -= s1 * zi;
          b[j + 2] -= s2 * zi;
        }
    }

  return comp_status::SUCCESS;
}

static void
cholesky_block_subtract (const size_t m, const size_t l, const size_t h,
                         const double *left, const double *right, const int *d,
                         double *block, const bool diagonal)
{
  const size_t m3 = m % 3, l3 = l % 3;
  size_t i = 0;

  for (; i < m3; ++i)
    {
      const double *p = left + i * h;
      double *row = block + i * l;
      size_t j = diagonal ? i : 0;

      for (; j < l3; ++j)
        {
          const double *q = right + j * h;
          double el = 0;
          for (size_t k = 0; k < h; ++k)
            el += p[k] * q[k] * d[k];
          row[j] -= el;
        }

      for (; j < l; j += 3)
        {
          const double *q0 = right + j * h;
          const double *q1 = q0 + h;
          const double *q2 = q1 + h;
          double s00 = 0, s01 = 0, s02 = 0;

          for (size_t k = 0; k < h; ++k)
            {
              double el = p[k] * d[k];
              s00 += el * q0[k];
              s01 += el * q1[k];
              s02 += el * q2[k];
            }

          row[j] -= s00;
          row[j + 1] -= s01;
          row[j + 2] -= s02;
        }
    }

  for (; i < m; i += 3)
    {
      const double *p0 = left + i * h;
      const double *p1 = p0 + h;
      const double *p2 = p1 + h;
      double *row0 = block + i * l;
      double *row1 = row0 + l;
      double *row2 = row1 + l;
      size_t j = 0;

      if (diagonal)
        {
          double s00 = 0, s01 = 0, s02 = 0;
          double s11 = 0, s12 = 0, s22 = 0;

          for (size_t k = 0; k < h; ++k)
            {
              double x0 = p0[k], x1 = p1[k], x2 = p2[k];
              double u0 = x0 * d[k];
              double u1 = x1 * d[k];
              double u2 = x2 * d[k];
              s00 += u0 * x0;
              s01 += u0 * x1;
              s02 += u0 * x2;
              s11 += u1 * x1;
              s12 += u1 * x2;
              s22 += u2 * x2;
            }

          row0[i] -= s00;
          row0[i + 1] -= s01;
          row0[i + 2] -= s02;
          row1[i + 1] -= s11;
          row1[i + 2] -= s12;
          row2[i + 2] -= s22;
          j = i + 3;
        }

      for (; j < l3; ++j)
        {
          const double *q = right + j * h;
          double s00 = 0, s10 = 0, s20 = 0;

          for (size_t k = 0; k < h; ++k)
            {
              double el = q[k] * d[k];
              s00 += p0[k] * el;
              s10 += p1[k] * el;
              s20 += p2[k] * el;
            }

          row0[j] -= s00;
          row1[j] -= s10;
          row2[j] -= s20;
        }

      for (; j < l; j += 3)
        {
          const double *q0 = right + j * h;
          const double *q1 = q0 + h;
          const double *q2 = q1 + h;
          double s00 = 0, s01 = 0, s02 = 0;
          double s10 = 0, s11 = 0, s12 = 0;
          double s20 = 0, s21 = 0, s22 = 0;

          for (size_t k = 0; k < h; ++k)
            {
              double u0 = p0[k] * d[k];
              double u1 = p1[k] * d[k];
              double u2 = p2[k] * d[k];
              double v0 = q0[k], v1 = q1[k], v2 = q2[k];
              s00 += u0 * v0;
              s01 += u0 * v1;
              s02 += u0 * v2;
              s10 += u1 * v0;
              s11 += u1 * v1;
              s12 += u1 * v2;
              s20 += u2 * v0;
              s21 += u2 * v1;
              s22 += u2 * v2;
            }

          row0[j] -= s00;
          row0[j + 1] -= s01;
          row0[j + 2] -= s02;
          row1[j] -= s10;
          row1[j + 1] -= s11;
          row1[j + 2] -= s12;
          row2[j] -= s20;
          row2[j + 1] -= s21;
          row2[j + 2] -= s22;
        }
    }
}

static comp_status
cholesky_block_method (const size_t n, const size_t m, double *a, int *d,
                       double *x, double *work, const double norm)
{
  const size_t l = n % m, end = n - l;
  const size_t size = m * m;
  double *diagonal = work;
  double *block = diagonal + size;
  double *left = block + size;
  double *result = left + size;

  for (size_t si = 0; si < end; si += m)
    {
      get_block_row (n, a, m, m, diagonal, si, si);
      comp_status res
          = cholesky_decomposition (m, diagonal, d + si, x + si, norm);
      if (res != comp_status::SUCCESS)
        return res;
      set_block_row (n, a, m, m, diagonal, si, si);

      for (size_t sj = si + m; sj < end; sj += m)
        {
          get_block_column (n, a, m, m, block, si, sj);
          res = cholesky_block_row (m, m, diagonal, d + si, block, x + si,
                                    x + sj, norm);
          if (res != comp_status::SUCCESS)
            return res;
          set_block_column (n, a, m, m, block, si, sj);

          for (size_t sk = si + m; sk < sj; sk += m)
            {
              get_block_column (n, a, m, m, left, si, sk);
              get_block_row (n, a, m, m, result, sk, sj);
              cholesky_block_subtract (m, m, m, left, block, d + si, result,
                                       false);
              set_block_row (n, a, m, m, result, sk, sj);
            }

          get_block_row (n, a, m, m, result, sj, sj);
          cholesky_block_subtract (m, m, m, block, block, d + si, result,
                                   true);
          set_block_row (n, a, m, m, result, sj, sj);
        }

      if (l != 0)
        {
          get_block_column (n, a, m, l, block, si, end);
          res = cholesky_block_row (m, l, diagonal, d + si, block, x + si,
                                    x + end, norm);
          if (res != comp_status::SUCCESS)
            return res;
          set_block_column (n, a, m, l, block, si, end);

          for (size_t sk = si + m; sk < end; sk += m)
            {
              get_block_column (n, a, m, m, left, si, sk);
              get_block_row (n, a, m, l, result, sk, end);
              cholesky_block_subtract (m, l, m, left, block, d + si, result,
                                       false);
              set_block_row (n, a, m, l, result, sk, end);
            }

          get_block_row (n, a, l, l, result, end, end);
          cholesky_block_subtract (l, l, m, block, block, d + si, result,
                                   true);
          set_block_row (n, a, l, l, result, end, end);
        }
    }

  if (l != 0)
    {
      get_block_row (n, a, l, l, diagonal, end, end);
      comp_status res
          = cholesky_decomposition (l, diagonal, d + end, x + end, norm);
      if (res != comp_status::SUCCESS)
        return res;
      set_block_row (n, a, l, l, diagonal, end, end);
    }

  return comp_status::SUCCESS;
}

/* При d == nullptr знаки уже применены.  */
static void
cholesky_vector_subtract (const size_t m, const size_t l, const double *block,
                          const double *x, double *b, const int *d)
{
  const size_t m3 = m % 3;
  size_t i = 0;

  for (; i < m3; ++i)
    {
      const double *p = block + i * l;
      double el = 0;
      for (size_t k = 0; k < l; ++k)
        el += p[k] * x[k];
      if (d != nullptr)
        b[i] = b[i] * d[i] - el;
      else
        b[i] -= el;
    }

  for (; i < m; i += 3)
    {
      const double *p0 = block + i * l;
      const double *p1 = p0 + l;
      const double *p2 = p1 + l;
      double s0 = 0, s1 = 0, s2 = 0;

      for (size_t k = 0; k < l; ++k)
        {
          double el = x[k];
          s0 += p0[k] * el;
          s1 += p1[k] * el;
          s2 += p2[k] * el;
        }

      if (d != nullptr)
        {
          b[i] = b[i] * d[i] - s0;
          b[i + 1] = b[i + 1] * d[i + 1] - s1;
          b[i + 2] = b[i + 2] * d[i + 2] - s2;
        }
      else
        {
          b[i] -= s0;
          b[i + 1] -= s1;
          b[i + 2] -= s2;
        }
    }
}

static comp_status
cholesky_backward_block (const size_t m, const double *r, const int *d,
                         double *x, const double norm)
{
  size_t i = m;

  if (d != nullptr && i != 0)
    {
      --i;
      const double *p = r + i * m;
      if (std::abs (p[i]) < SQRT_DBL_EPS * norm)
        return comp_status::ERROR_DBZ;

      double el = (x[i] * d[i]) / p[i];
      x[i] = el;
      size_t j = 0;

      for (; j + 2 < i; j += 3)
        {
          x[j] = x[j] * d[j] - p[j] * el;
          x[j + 1] = x[j + 1] * d[j + 1] - p[j + 1] * el;
          x[j + 2] = x[j + 2] * d[j + 2] - p[j + 2] * el;
        }
      for (; j < i; ++j)
        x[j] = x[j] * d[j] - p[j] * el;
    }

  for (; i != 0;)
    {
      --i;
      const double *p = r + i * m;
      if (std::abs (p[i]) < SQRT_DBL_EPS * norm)
        return comp_status::ERROR_DBZ;

      double el = x[i] / p[i];
      x[i] = el;
      size_t j = 0;

      for (; j + 2 < i; j += 3)
        {
          x[j] -= p[j] * el;
          x[j + 1] -= p[j + 1] * el;
          x[j + 2] -= p[j + 2] * el;
        }
      for (; j < i; ++j)
        x[j] -= p[j] * el;
    }

  return comp_status::SUCCESS;
}

static comp_status
cholesky_backward (const size_t n, const size_t m, const double *a,
                   const int *d, double *x, double *work, const double norm)
{
  const size_t l = n % m, end = n - l;
  double *diagonal = work;
  double *block = diagonal + m * m;
  size_t sj = end;

  if (l != 0)
    {
      get_block_row (n, a, l, l, diagonal, end, end);
      comp_status res
          = cholesky_backward_block (l, diagonal, d + end, x + end, norm);
      if (res != comp_status::SUCCESS)
        return res;

      for (size_t si = end; si != 0;)
        {
          si -= m;
          get_block_row (n, a, m, l, block, si, end);
          cholesky_vector_subtract (m, l, block, x + end, x + si, d + si);
        }
    }
  else if (sj != 0)
    {

      sj -= m;
      get_block_row (n, a, m, m, diagonal, sj, sj);
      comp_status res
          = cholesky_backward_block (m, diagonal, d + sj, x + sj, norm);
      if (res != comp_status::SUCCESS)
        return res;

      for (size_t si = sj; si != 0;)
        {
          si -= m;
          get_block_row (n, a, m, m, block, si, sj);
          cholesky_vector_subtract (m, m, block, x + sj, x + si, d + si);
        }
    }

  for (; sj != 0;)
    {
      sj -= m;
      get_block_row (n, a, m, m, diagonal, sj, sj);
      comp_status res
          = cholesky_backward_block (m, diagonal, nullptr, x + sj, norm);
      if (res != comp_status::SUCCESS)
        return res;

      for (size_t si = sj; si != 0;)
        {
          si -= m;
          get_block_row (n, a, m, m, block, si, sj);
          cholesky_vector_subtract (m, m, block, x + sj, x + si, nullptr);
        }
    }

  return comp_status::SUCCESS;
}

comp_status
cholesky_method (const size_t n, const size_t m, double *a, double *b, int *d,
                 double *work, const double norm)
{
  comp_status res = cholesky_block_method (n, m, a, d, b, work, norm);
  if (res != comp_status::SUCCESS)
    return res;

  return cholesky_backward (n, m, a, d, b, work, norm);
}
