#include "solve.h"

#include <cstdio>
#include <ctime>

int
main (int argc, char *argv[])
{
  int res = 0, k;
  double t;

  if (!(argc == 3 && sscanf (argv[2], "%d", &k)))
    {
      printf ("%s : filename k\n", argv[0]);
      return -2;
    }

  t = clock ();
  res = t1_solve (argv[1], k);
  t = (clock () - t) / CLOCKS_PER_SEC;

  if (res < 0)
    {
      fprintf(stderr, "%s : %d\n", argv[0], res);
      return -1;
    }

  fprintf (stdout, "%s : %d\n", argv[0], res);

  return 0;
}
