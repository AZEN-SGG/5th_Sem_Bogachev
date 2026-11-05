#include "solve.h"

double
t1_solve (double (*f) (double), double a, double b, int n)
{
  const double h = (b - a) / n;
  double x = a;
  double sum = (f (a) + f (b)) * 0.5;

  if (h < NUM_FPE)
    return DBL_MAX;

  for (int i = 1; i < n; ++i)
    {
      x += h;
      sum += f (x);
    }

  return h * sum;
}
