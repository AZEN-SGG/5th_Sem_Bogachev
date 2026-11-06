#include "solve_09.h"

int
t9_solve (double (*f) (double), double a, double b, double eps, int m,
          double *x)
{
  double c = a + (b - a) * 0.5;
  double y_a = f (a), y_c = f (c), y_b = f (b);
  double prev = DBL_MAX;

  for (int it = 1; it <= m; ++it)
    {
      if (y_a > y_c)
        {
          b = c;
          y_b = y_c;
          c = a;
          y_c = y_a;
          a = c - 2 * (b - c);
          y_a = f (a);
          continue;
        }
      if (y_b > y_c)
        {
          a = c;
          y_a = y_c;
          c = b;
          y_c = y_b;
          b = c + 2 * (c - a);
          y_b = f (b);
          continue;
        }

      if (b - a - eps < DBL_EPSILON)
        {
          *x = c;
          return it;
        }

      double s0 = (y_c - y_a) / (c - a);
      double s1 = (y_b - y_c) / (b - c);
      double q = (s1 - s0) / (b - a);
      if (!(q < 0))
        return -1;

      double u = a + (c - a) * 0.5 - s0 / (2 * q);
      if (fabs (u - prev) - eps < DBL_EPSILON)
        {
          *x = u;
          return it;
        }
      prev = u;

      if (fabs (u - c) - eps < DBL_EPSILON)
        {
          if (c - a > b - c)
            u = a + (c - a) * 0.5;
          else
            u = c + (b - c) * 0.5;
        }

      double y = f (u);
      if (u < c)
        {
          if (y > y_c)
            {
              b = c;
              y_b = y_c;
              c = u;
              y_c = y;
            }
          else
            {
              a = u;
              y_a = y;
            }
        }
      else
        {
          if (y > y_c)
            {
              a = c;
              y_a = y_c;
              c = u;
              y_c = y;
            }
          else
            {
              b = u;
              y_b = y;
            }
        }
    }

  return -1;
}
