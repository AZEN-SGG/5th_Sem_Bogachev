#include "solve.h"

#include <cstdio>

int
main (int argc, char *argv[])
{
  int res = 0, k;

  if (!(argc == 3 && sscanf (argv[2], "%d", &k)))
    {
      printf ("%s : filename k\n", argv[0]);
      return -2;
    }

  res = t1_solve (argv[1], k);

  if (res < 0)
    {
      fprintf (stderr, "%s : %d\n", argv[0], res);
      return -1;
    }

  fprintf (stdout, "%s : %d\n", argv[0], res);

  return 0;
}
