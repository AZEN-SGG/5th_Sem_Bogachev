#include "solve_08.h"

int
t8_solve (double (*f) (double), double x_0, double eps, int m, double *x)
{
  for (int it = 1; it <= m; ++it)
    {
      double y = f (x_0);
      if (fabs (y - x_0) - eps < DBL_EPSILON)
        {
          *x = x_0;
          return it;
        }

      x_0 = y;
    }

  return -1;
}
