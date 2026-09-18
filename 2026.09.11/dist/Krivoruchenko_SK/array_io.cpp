#include "array_io.h"
#include <cstdio>

void
print_array (const int p, const int n, double *arr)
{
  int k = p < n ? p : n;
  for (int i = 0; i < k; ++i)
    fprintf (stdout, "%lf\n", arr[i]);

  return;
}

int
read_file (const char *filename, const int n, double *&arr)
{
  int i = 0;
  FILE *fp = fopen (filename, "r");

  if (!fp)
    return -1;

  arr = (double *)malloc (n * sizeof (double));
  if (!arr)
    {
      fclose (fp);
      return -2;
    }

  for (i = 0; i < n && fscanf (fp, "%lf", arr + i) == 1; i++)
    ;

  if (i != n)
    {
      free (arr);
      fclose (fp);
      return -3;
    }

  fclose (fp);
  return n;
}

int
gen_array (const int s, const int n, double *&arr)
{
  arr = (double *)malloc (n * sizeof (double));
  if (!arr)
    return -2;

  for (int i = 0; i < n; ++i)
    arr[i] = f (s, n, i + 1);

  return n;
}

double
f (const int s, const int n, const int i)
{
  double val = 0;

  switch (s)
    {
    case 1:
      val = i;
      break;

    case 2:
      val = n - i;
      break;

    case 3:
      val = i * 0.5;
      break;

    case 4:
      val = n - i * 0.5;
      break;

    case 5:
      val = 2 * i;
      break;

    case 6:
      val = n - 2 * i;
      break;
    }

  return val;
}
