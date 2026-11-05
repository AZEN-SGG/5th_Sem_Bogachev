#include "init_f.h"

double
f1 (int n, int i, int j)
{
  return n - MAX (i, j) + 1;
}

double
f2 (int n, int i, int j)
{
  (void)n;
  return MAX (i, j);
}

double
f3 (int n, int i, int j)
{
  (void)n;
  return abs (i - j);
}

double
f4 (int n, int i, int j)
{
  (void)n;
  return 1. / (i + j - 1);
}
