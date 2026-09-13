#include "io_status.h"
#include "solve.h"

#include <cstdio>
#include <ctime>

int
main (int argc, char *argv[])
{
  io_status ret;
  int res = 0, task = 1, k;
  double t;

  if (!(argc == 3 && sscanf (argv[2], "%d", &k)))
    {
      printf ("Usage %s filename k\n", argv[0]);
      return 1;
    }

  t = clock ();
  ret = t1_solve (argv[1], k, res);
  t = (clock () - t) / CLOCKS_PER_SEC;

  do
    {
      switch (ret)
        {
        case io_status::success:
          continue;
        case io_status::open:
          fprintf (stderr, "%s: cannot open file %s\n", argv[0], argv[1]);
          return -1;
        case io_status::format:
          fprintf (stderr, "%s: wrong format of file %s\n", argv[0], argv[1]);
          return -2;
        case io_status::eof:
          fprintf (stderr, "%s: end of file %s\n", argv[0], argv[1]);
          return -3;
        case io_status::memory:
          fprintf (stderr, "%s: memory\n", argv[0]);
          return -4;
        case io_status::create:
          fprintf (
              stderr,
              "%s: create, how is it possible?!\n", argv[0]);
          return -5;
        }

      return -1;
    }
  while (0);

  fprintf (stdout, "%s : Task = %d Result = %d Elapsed = %.2f\n", argv[0],
           task, res, t);

  return 0;
}
