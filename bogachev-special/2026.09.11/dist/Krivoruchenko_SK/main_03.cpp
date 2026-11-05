#include "solve_03.h"

#include <cstdio>

int
main (int argc, char *argv[])
{
  int res;

  if (!(argc == 2))
    {
      printf ("%s : filename\n", argv[0]);
      return -2;
    }

  res = t3_solve (argv[1]);

  if (res < 0)
    {
      fprintf (stderr, "%s : %d\n", argv[0], res);
      return -1;
    }

  fprintf (stdout, "%s : %d\n", argv[0], res);

  return 0;
}
