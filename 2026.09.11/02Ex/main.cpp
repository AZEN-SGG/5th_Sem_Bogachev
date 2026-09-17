#include "solve.h"

#include <cstdio>
#include <ctime>

int
main (int argc, char *argv[])
{
  int res;
  double t;

  if (!(argc == 3))
    {
      printf ("%s : file_a, file_b\n", argv[0]);
      return -2;
    }

  t = clock ();
  res = t2_solve (argv[1], argv[2]);
  t = (clock () - t) / CLOCKS_PER_SEC;

  if (res < 0)
    {
      fprintf (stderr, "%s : %d\n", argv[0], res);
      return -1;
    }

  fprintf (stdout, "%s : %d\n", argv[0], res);

  return 0;
}
