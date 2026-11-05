#include "solve_02.h"
#include <cstdio>

int
t2_solve (const char *file_a, const char *file_b)
{
  int ret;
  double a_before, a_curr, a_after, b_curr, b_after;

  FILE *fp_b, *fp_a = fopen (file_a, "r");
  if (!fp_a)
    return -1;

  fp_b = fopen (file_b, "r");
  if (!fp_b)
    {
      fclose (fp_a);
      return -1;
    }

  if (!((fscanf (fp_a, "%lf", &a_before) == 1)
        && (fscanf (fp_a, "%lf", &a_curr) == 1)
        && (fscanf (fp_b, "%lf", &b_curr) == 1)
        && (fscanf (fp_b, "%lf", &b_curr) == 1)))
    {
      fclose (fp_a);
      fclose (fp_b);
      return 1;
    }

  while (1)
    {
      if (fscanf (fp_b, "%lf", &b_after) != 1)
        {
          if (!feof (fp_b))
            ret = -3;
          else
            ret = 1;
          break;
        }
      if (fscanf (fp_a, "%lf", &a_after) != 1)
        {
          ret = -2;
          break;
        }

      if (std::abs ((b_curr * 2) - (a_before + a_after)) > EPS)
        {
          ret = 0;
          break;
        }

      a_before = a_curr;
      a_curr = a_after;
      b_curr = b_after;
    }

  fclose (fp_a);
  fclose (fp_b);
  return ret;
}
