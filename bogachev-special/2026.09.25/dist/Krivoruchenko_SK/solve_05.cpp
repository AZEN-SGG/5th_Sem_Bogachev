#include "solve_05.h"

int
t5_solve (double (*f) (double), double (*d) (double), double x_0, double eps,
          int m, double *x)
{
  double frac;
  int it = 0;

  double y = f (x_0);
  double dy = d (x_0);

  if (fabs (dy) < DBL_EPSILON)
    return -1;

  frac = y / dy;

  if (fabs (frac) < eps + DBL_EPSILON)
    {
      *x = x_0;
      return it;
    }

  for (it = 1; it <= m; ++it)
    {
      x_0 -= frac;

      y = f (x_0);
      dy = d (x_0);

      if (fabs (dy) < DBL_EPSILON)
        {
          it = -1;
          break;
        }

      frac = y / dy;

      if (fabs (frac) < eps + DBL_EPSILON)
        break;
    }

  if (it > m)
    it = -1;

  *x = x_0;
  return it;
}
