#include "solve.h"
#include <cstdio>

int
t3_solve (const char *filename)
{
  int type = 4;
  double a_before, a_curr, b, q = 1;

  FILE *fp = fopen (filename, "r");
  if (!fp)
    return -1;

  if (fscanf (fp, "%lf", &a_before) != 1)
    {
      fclose (fp);
      return type;
    }

  if (fscanf (fp, "%lf", &a_curr) == 1)
    {
      b = a_curr - a_before;
      if (std::abs (a_before) > EPS)
        q = a_curr / a_before;
      else
        type = 1;
    }
  else
    {
      fclose (fp);
      return type;
    }

  a_before = a_curr;

  while (fscanf (fp, "%lf", &a_curr) == 1)
    {
      if (type == 4 || type == 1)
        {
          if (std::abs (b - (a_curr - a_before)) > EPS)
            {
              if (type == 1)
                {
                  type = 0;
                  break;
                }
            }
          else
            type = 1;
        }

      if (type == 4 || type == 2)
        {
          if (std::abs (q * a_before - a_curr) > EPS)
            {
              type = 0;
              break;
            }

          type = 2;
        }

      a_before = a_curr;
    }

  if ((type == 1 || type == 4) && std::abs (b) < EPS)
    type = 3;

  fclose (fp);
  return type;
}
