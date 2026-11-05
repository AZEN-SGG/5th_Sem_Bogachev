#include "solve_01.h"

/* The Lagrange interpolation polynomial.  */
double
t1_solve (const double x_0, const int n, const double *X,
          const double *Y)
{
  double value = 0;

  for (int i = 0; i < n; ++i)
    {
      double x_i = X[i];
      double y_i = Y[i];

      double numerator = 1;
      double denominator = 1;

      if (fabs (x_0 - x_i) < DBL_EPSILON)
        return y_i;

      for (int j = 0; j < i; ++j)
        {
          double x_j = X[j];
          numerator *= (x_0 - x_j);
          denominator *= (x_i - x_j);
        }

      for (int j = i + 1; j < n; ++j)
        {
          double x_j = X[j];
          numerator *= (x_0 - x_j);
          denominator *= (x_i - x_j);
        }

      if (fabs (denominator) < DBL_EPSILON)
        return DBL_MAX;

      numerator *= y_i;

      value += numerator / denominator;
    }

  return value;
}
