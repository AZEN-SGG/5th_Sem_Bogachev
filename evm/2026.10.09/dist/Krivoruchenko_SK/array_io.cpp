#include "array_io.h"
#include "io_status.h"
#include <cmath>
#include <cstdio>
#include <cstring>

io_status
read_matrix (const size_t n, double *a, const char *filename)
{
  size_t index = 0;
  double temp;
  FILE *fp = fopen (filename, "r");

  if (!fp)
    return ERROR_OPEN;

  for (size_t i = 0; i < n; i++)
    for (size_t j = 0; j < n; j++)
      {
        if (fscanf (fp, "%lf", &temp) == 1)
          {
            a[index++] = temp;
            if (j < i && std::abs (temp - a[j * n + i]) > 0)
              {
                fclose (fp);
                return ERROR_SYMMETRY;
              }
          }
        else
          {
            fclose (fp);
            return ERROR_READ;
          }
      }

  fclose (fp);
  return SUCCESS;
}

void
print_matrix (size_t n, const double *a, size_t r)
{
  FILE *fp = stdout;
  size_t np = (n > r ? r : n);

  for (size_t i = 0; i < np; i++)
    {
      for (size_t j = 0; j < np; j++)
        fprintf (fp, " %10.3e", a[i * n + j]);
      fprintf (fp, "\n");
    }
}

void
init_matrix (const size_t n, double *a, const int s)
{
  double (*q) (int, int, int);
  double (*f[]) (int, int, int) = { f1, f2, f3, f4 };
  size_t i, j;
  q = f[s - 1];
  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++)
      a[i * n + j] = q (n, i + 1, j + 1);
}

void
init_identity_matrix (double *a, size_t n)
{
  memset (a, 0, n * n * sizeof (double));

  for (size_t i = 0; i < n; ++i)
    a[i * n + i] = 1.0;
}

int
read_or_init_matrix (const size_t n, double *a, const char *filename,
                     const int s)
{
  if (filename)
    { /* из файла */
      io_status ret;
      ret = read_matrix (n, a, filename);
      do
        {
          switch (ret)
            {
            case SUCCESS:
              continue;
            case ERROR_OPEN:
              fprintf (stderr, "Cannot open %s\n", filename);
              break;
            case ERROR_READ:
            case ERROR_FORMAT:
              fprintf (stderr, "Cannot read %s\n", filename);
              break;
            case ERROR_MEM:
              fprintf (stderr, "Not enough memory\n");
              break;
            case ERROR_SYMMETRY:
              fprintf (stderr, "The matrix in %s is not symmetric\n",
                       filename);
              break;
            }
          return 3;
        }
      while (0);
    }
  else
    init_matrix (n, a, s);

  return 0;
}

void
print_vector (const size_t n, const double *vec, const size_t r)
{
  FILE *fp = stdout;
  const size_t np = (n > r ? r : n);

  for (size_t i = 0; i < np; i++)
    fprintf (fp, " %10.3e", vec[i]);

  fprintf (fp, "\n");
}
