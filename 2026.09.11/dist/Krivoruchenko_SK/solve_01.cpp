#include "solve_01.h"

int
t1_solve (const char *filename, const int k)
{
  const int len_int = sizeof (int) << 3;
  int i, n, len_plot = 0, dist = 0, min = -1;

  FILE *fp = fopen (filename, "r");
  if (!fp)
    return -2;

  while (fscanf (fp, "%d", &n) == 1)
    {
      /* Find the first nonzero bit, skipping leading zeroes.  */
      for (i = len_int - 1; ((n >> i) & 1) != 1 && i >= 0; i--)
        ;
      for (; i >= 0; i--)
        {
          if (((n >> i) & 1) == 1)
            len_plot++;
          else
            {
              if (len_plot >= k)
                {
                  if (min * dist < 0 || dist < min)
                    min = dist;
                  dist = 1;
                }
              else if (dist)
                dist += len_plot + 1;
              len_plot = 0;
            }
        }
    }

  if (len_plot >= k && (min * dist < 0 || dist < min))
    min = dist;

  fclose (fp);
  return min;
}
