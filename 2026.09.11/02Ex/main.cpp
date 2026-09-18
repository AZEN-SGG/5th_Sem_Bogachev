#include "solve.h"

#include <cstdio>

int
main (int argc, char *argv[])
{
  int res;

  if (!(argc == 3))
    {
      printf ("%s : file_a, file_b\n", argv[0]);
      return -2;
    }

  res = t2_solve (argv[1], argv[2]);

  if (res < 0)
    {
      fprintf (stderr, "%s : %d\n", argv[0], res);
      return -1;
    }

  fprintf (stdout, "%s : %d\n", argv[0], res);

  return 0;
}
