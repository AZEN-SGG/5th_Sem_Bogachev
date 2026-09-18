#include "array_io.h"
#include "solve_07.h"

#include <cstdio>
#include <ctime>

int
main (int argc, char *argv[])
{
  int res, n, p, s;
  double t, *arr;

  if (!((argc == 5 || argc == 6) && sscanf (argv[2], "%d", &n) && n > 0
        && sscanf (argv[3], "%d", &p) && sscanf (argv[4], "%d", &s)
        && ((s == 0 && argc == 6) || (s > 0 && s < 7))))
    {
      printf ("%s : k n p s [filename]\n", argv[0]);
      return -2;
    }

  if (argc == 6)
    res = read_file (argv[5], n, arr);
  else
    res = gen_array (s, n, arr);

  if (res < 0)
    {
      fprintf (stderr, "%s : %d\n", argv[0], res);
      return res;
    }

  print_array (p, n, arr);
  fprintf (stdout, "\n");

  t = clock ();
  res = t7_solve (argv[1], arr, n);
  t = (clock () - t) / CLOCKS_PER_SEC;

  if (res < 0)
    fprintf (stderr, "%s : %d\n", argv[0], res);
  else
    fprintf (stdout, "%s : %d\n", argv[0], res);
  fprintf (stdout, "\nTime = %.2f\n", t);

  free (arr);
  return 0;
}
