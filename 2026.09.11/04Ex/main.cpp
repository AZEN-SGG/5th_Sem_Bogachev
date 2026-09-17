#include "solve.h"

#include <cstdio>
#include <ctime>

int
main (int argc, char *argv[])
{
  int res;
  double t, d;

  if (!(argc == 2))
    {
      printf ("%s : filename\n", argv[0]);
      return -2;
    }

  t = clock ();
  res = t4_solve (argv[1], d);
  t = (clock () - t) / CLOCKS_PER_SEC;

  if (res < 0)
    {
      fprintf (stderr, "%s : %d\n", argv[0], res);
      return -1;
    }

  fprintf (stdout, "%s : %d - %lf\n", argv[0], res, d);

  return 0;
}
