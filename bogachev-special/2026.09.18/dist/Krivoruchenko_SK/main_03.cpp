#include "solve_03.h"

#include <cstdio>
#include <ctime>

int
main (int argc, char *argv[])
{
  int task = 3;
  const char *f_in = 0;
  const char *f_out = 0;
  const char *s = 0;
  int res = 0;
  double t;

  if (argc != 4)
    {
      printf ("Usage: %s <f_in> <f_out> <s>\n", argv[0]);
      return 1;
    }
  f_in = argv[1];
  f_out = argv[2];
  s = argv[3];
  t = clock ();
  res = t3_solve (f_in, f_out, s);
  t = (clock () - t) / CLOCKS_PER_SEC;
  printf ("%s : Task = %d Result = %d Elapsed = %.2f\n", argv[0], task, res,
          t);
  return res < 0 ? 1 : 0;
}
