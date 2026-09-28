#include "integral.h"

int
trapezoid (double (*f) (double), double a, double b, double eps, double *res)
{
  int it;

  double n = 1;
  double h = (b - a);
  double integ_n = (f (a) + f (b)) * h * 0.5;

  for (it = 1; it <= MAX_ITER; ++it)
    {
      double x = a + h * 0.5;
      double integ_2n = 0;

      for (int i = 0; i < n; i++)
        {
          integ_2n += f (x);
          x += h;
        }

      h *= 0.5;
      integ_2n = integ_2n * h + integ_n * 0.5;

      if (fabs (integ_2n - integ_n) < eps)
        break;

      integ_n = integ_2n;
      n *= 2;
    }

  if (it > MAX_ITER)
    return -1;

  *res = integ_n;

  return n;
}

int
simpson (double (*f) (double), double a, double b, double eps, double *res)
{
  int it;
  double integ_n;

  int n = 2;
  double h = (b - a) * 0.5;
  double s1 = (f (a) + f (b)) * h / 3;
  double s2 = f (a + h) * h * 4 / 3;

  for (it = 1; it <= MAX_ITER; ++it)
    {
      double x = a + h * 0.5;
      double s2_2n = 0;

      for (int i = 0; i < n; i++)
        {
          s2_2n += f (x);
          x += h;
        }

      s2_2n *= h * 2 / 3;

      if (fabs ((s2_2n - s1 * 0.5) - s2 * 0.75) < eps)
        {
          integ_n = s1 + s2;
          break;
        }

      s1 = s1 * 0.5 + s2 * 0.25;
      s2 = s2_2n;

      h *= 0.5;
      n <<= 1;
    }

  if (it > MAX_ITER)
    return -1;

  *res = integ_n;

  return n;
}
