#include "solve.h"
#include <cstdio>

int
t4_solve (const char *filename, double &d)
{
  int n = 0;
  double sum = 0, sum_sq = 0, el;

  FILE *fp = fopen (filename, "r");
  if (!fp)
    return -1;

  for (n = 0; fscanf (fp, "%lf", &el) == 1 ; n++, sum += el, sum_sq += el * el)
    ;

  d = (sum_sq / n) - ((sum * sum) / (n * n));

  fclose (fp);
  return n;
}
