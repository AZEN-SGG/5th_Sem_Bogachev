#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <new>

#include "array_io.h"
#include "matrix.h"
#include "solve.h"

/* ./a.out n m r s [filename].  */
int
main (int argc, char *argv[])
{
  double t1 = 0, t2 = 0, *a, *b, *x, *work, r1 = -1, r2 = -1;
  int s, res = 0, *d, task = 5;
  size_t n, m, r;
  char *filename = 0;
  comp_status ret;

  if (!((argc == 5 || argc == 6) && sscanf (argv[1], "%zu", &n) == 1
        && sscanf (argv[2], "%zu", &m) == 1 && sscanf (argv[3], "%zu", &r) == 1
        && sscanf (argv[4], "%d", &s) == 1 && (s >= 0 && s <= 4)
        && (!(s == 0 && argc == 5))))
    {
      fprintf (stderr, "Usage: %s n m r s [filename]\n", argv[0]);
      return 1;
    }

  if (argc == 6 && s == 0)
    filename = argv[5];

  a = new (std::nothrow) double[n * n];
  if (!a)
    {
      fprintf (stderr, "Not enough memory\n");
      return 2;
    }

  b = new (std::nothrow) double[n];
  if (!b)
    {
      delete[] a;
      fprintf (stderr, "Not enough memory\n");
      return 2;
    }

  x = new (std::nothrow) double[n];
  if (!x)
    {
      delete[] a;
      delete[] b;
      fprintf (stderr, "Not enough memory\n");
      return 2;
    }

  d = new (std::nothrow) int[n];
  if (!d)
    {
      delete[] a;
      delete[] b;
      delete[] x;
      fprintf (stderr, "Not enough memory\n");
      return 2;
    }

  work = new (std::nothrow) double[4 * m * m];
  if (!work)
    {
      delete[] a;
      delete[] b;
      delete[] x;
      delete[] d;
      fprintf (stderr, "Not enough memory\n");
      return 2;
    }

  res = read_or_init_matrix (n, a, filename, s);
  if (res)
    {
      delete[] a;
      delete[] b;
      delete[] x;
      delete[] d;
      delete[] work;

      fprintf (stdout,
               "%s : Task = %d Res1 = %e Res2 = %e T1 = %.2f T2 = %.2f S = %d "
               "N = %zu M = %zu\n",
               argv[0], task, r1, r2, t1, t2, s, n, m);

      return res;
    }

  init_vec_b (n, a, b);

  fprintf (stdout, "Initial matrix:\n");
  print_matrix (n, a, r);

  fprintf (stdout, "Vector b:\n");
  print_vector (n, b, r);

  t1 = clock ();
  ret = t5_solve (n, m, a, b, d, work);
  t1 = (clock () - t1) / CLOCKS_PER_SEC;

  do
    {
      switch (ret)
        {
        case comp_status::SUCCESS:
          continue;
        case comp_status::ERROR_DBZ:
          fprintf (stderr, "Cholesky: zero or too small diagonal element\n");
          break;
        case comp_status::ERROR_SINGULAR:
          fprintf (stderr, "The matrix is degenerate\n");
          break;
        }

      delete[] a;
      delete[] b;
      delete[] x;
      delete[] d;
      delete[] work;

      fprintf (stdout,
               "%s : Task = %d Res1 = %e Res2 = %e T1 = %.2f T2 = %.2f S = %d "
               "N = %zu M = %zu\n",
               argv[0], task, r1, r2, t1, t2, s, n, m);

      return 4;
    }
  while (0);

  for (size_t i = 0; i < n; ++i)
    x[i] = b[i];

  res = read_or_init_matrix (n, a, filename, s);
  if (res)
    {
      delete[] a;
      delete[] b;
      delete[] x;
      delete[] d;
      delete[] work;

      fprintf (stdout,
               "%s : Task = %d Res1 = %e Res2 = %e T1 = %.2f T2 = %.2f S = %d "
               "N = %zu M = %zu\n",
               argv[0], task, r1, r2, t1, t2, s, n, m);

      return res;
    }
  init_vec_b (n, a, b);

  t2 = clock ();
  r1 = get_r1 (n, m, a, b, x, work);

  r2 = get_r2 (n, x);
  t2 = (clock () - t2) / CLOCKS_PER_SEC;

  fprintf (stdout, "Solution:\n");
  print_vector (n, x, r);

  fprintf (stdout, "\n");
  fprintf (stdout,
           "%s : Task = %d Res1 = %e Res2 = %e T1 = %.2f T2 = %.2f S = %d N = "
           "%zu M = %zu\n",
           argv[0], task, r1, r2, t1, t2, s, n, m);

  delete[] a;
  delete[] b;
  delete[] x;
  delete[] d;
  delete[] work;

  return 0;
}
