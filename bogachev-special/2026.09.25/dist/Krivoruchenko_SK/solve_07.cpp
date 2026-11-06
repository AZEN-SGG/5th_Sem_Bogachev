#include "solve_07.h"

int
t7_solve (double (*f) (double), double a, double b, double eps, int m,
          double *x)
{
  double c = a + (b - a) * 0.5;
  double y_a = f (a), y_c = f (c), y_b = f (b);

  if (fabs (y_a) - eps < DBL_EPSILON)
    {
      *x = a;
      return 1;
    }
  if (fabs (y_b) - eps < DBL_EPSILON)
    {
      *x = b;
      return 1;
    }
  if (fabs (y_c) - eps < DBL_EPSILON)
    {
      *x = c;
      return 1;
    }

  for (int it = 1; it <= m; ++it)
    {
      double d_ac = y_c - y_a;
      double d_cb = y_b - y_c;
      double d_ab = y_b - y_a;
      if (fabs (d_ac) < DBL_EPSILON || fabs (d_cb) < DBL_EPSILON
          || fabs (d_ab) < DBL_EPSILON)
        return -1;

      double s0 = (c - a) / d_ac;
      double s1 = (b - c) / d_cb;
      double u = a - s0 * y_a + ((s1 - s0) / d_ab) * y_a * y_c;
      double y = f (u);

      if (fabs (y) - eps < DBL_EPSILON)
        {
          *x = u;
          return it;
        }

      if (fabs (y_a) >= fabs (y_c) && fabs (y_a) >= fabs (y_b))
        {
          a = u;
          y_a = y;
        }
      else if (fabs (y_c) >= fabs (y_b))
        {
          c = u;
          y_c = y;
        }
      else
        {
          b = u;
          y_b = y;
        }
    }

  return -1;
}
