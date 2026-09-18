#include "solve_04.h"

#include <cstdio>

int
main (int argc, char *argv[])
{
  int res;
  double d;

  if (!(argc == 2))
    {
      printf ("%s : filename\n", argv[0]);
      return -2;
    }

  res = t4_solve (argv[1], d);

  if (res < 0)
    {
      fprintf (stderr, "%s : %d\n", argv[0], res);
      return -1;
    }

  fprintf (stdout, "%s : %d - %lf\n", argv[0], res, d);

  return 0;
}
