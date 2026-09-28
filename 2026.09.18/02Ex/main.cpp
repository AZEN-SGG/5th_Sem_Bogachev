#include "io_status.h"
#include "solve.h"
#include <stdio.h>
#include <time.h>

int
main (int argc, char *argv[])
{
  int task = 2;
  io_status ret;
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
  ret = t2_solve (f_in, f_out, s, &res);
  t = (clock () - t) / CLOCKS_PER_SEC;
  switch (ret)
    {
    case SUCCESS:
      printf ("%s : Task = %d Result = %d Elapsed = %.2f\n", argv[0], task,
              res, t);
      break;
    case ERROR_OPEN:
      printf ("Can not open %s\n", f_out);
      return 1;
    case ERROR_READ:
      printf ("Can not read %s\n", f_out);
      return 2;
    case ERROR_PATTERN:
      printf ("Error in pattern %s\n", s);
      return 3;
    }
  return 0;
}
