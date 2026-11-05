#include "solve_07.h"
#include <cstdio>

/* i — the element of arr that did not match in the file.  */
int
get_index (const int i, const double *arr)
{
  int j;

  /* Search for a starting element that matches the beginning of arr.  */
  for (j = 1; j < i; ++j)
    {
      if (std::abs (arr[j] - arr[0]) < EPS)
        {
          bool fit = true;

          /* Check that the subsequent sequence also matches.  */
          for (int k = 1; k < i - j; ++k)
            {
              if (std::abs (arr[j + k] - arr[k]) > EPS)
                {
                  fit = false;
                  break;
                }
            }

          if (fit)
            break;
        }
    }

  return i - j;
}

int
t7_solve (const char *filename, const double *arr, const int n)
{
  double el;
  int i = 0, count = 0;

  FILE *fp = fopen (filename, "r");
  if (!fp)
    return -1;

  while (fscanf (fp, "%lf", &el) == 1)
    {
      fprintf (stdout, "%lf\n", el);
      if (std::abs (arr[i] - el) < EPS)
        {
          if (i + 1 != n)
            i++;
          else
            {
              count++;
              i = get_index (n, arr);
            }

          continue;
        }
      else if (i == 0)
        continue;

      /* We have obtained a new suitable sequence. We are verifying it.  */
      do
        {
          i = get_index (i, arr);
          if (std::abs (arr[i] - el) < EPS)
            {
              i++;
              break;
            }
        }
      while (i != 0);
    }

  fprintf (stdout, "\n");

  fclose (fp);
  return count;
}
