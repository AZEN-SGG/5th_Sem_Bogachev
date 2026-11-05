#include "array_io.h"
#include "solve_08.h"

#include <cstdio>
#include <ctime>

int
main (int argc, char *argv[])
{
  int res, n, p_a, s_a, m, p_b, s_b;
  double t, *arr_a, *arr_b, *arr_c;

  if (!((argc > 6 && argc < 10) && sscanf (argv[1], "%d", &n) && n > 0
        && sscanf (argv[2], "%d", &p_a) && sscanf (argv[3], "%d", &s_a)
        && ((s_a == 0 && argc > 7) || (s_a > 0 && s_a < 7))
        && sscanf (argv[4 + (s_a == 0)], "%d", &m) && (m > 0)
        && sscanf (argv[5 + (s_a == 0)], "%d", &p_b)
        && sscanf (argv[6 + (s_a == 0)], "%d", &s_b)
        && ((s_b == 0 && ((s_a == 0 && argc == 9) || (s_a != 0 && argc == 8)))
            || (s_b > 0 && s_b < 7))))
    {
      printf ("%s : n p_a s_a [file_a] m p_b s_b [file_b]\n", argv[0]);
      return -2;
    }

  arr_c = (double *)malloc ((n + m) * sizeof (double));
  if (!arr_c)
    {
      fprintf (stderr, "%s : -2\n", argv[0]);
      return -2;
    }

  if (s_a == 0)
    res = read_file (argv[4], n, arr_a);
  else
    res = gen_array (s_a, n, arr_a);

  if (res < 0)
    {
      fprintf (stderr, "%s : %d\n", argv[0], res);
      free (arr_c);
      return res;
    }

  if (s_b == 0)
    res = read_file (argv[7 + (s_a == 0)], m, arr_b);
  else
    res = gen_array (s_b, m, arr_b);

  if (res < 0)
    {
      fprintf (stderr, "%s : %d\n", argv[0], res);
      free (arr_a);
      free (arr_c);
      return res;
    }

  print_array (p_a, n, arr_a);
  fprintf (stdout, "\n");
  print_array (p_b, m, arr_b);
  fprintf (stdout, "\n");

  t = clock ();
  res = t8_solve (arr_a, n, arr_b, m, arr_c);
  t = (clock () - t) / CLOCKS_PER_SEC;

  print_array (p_a + p_b, res, arr_c);
  fprintf (stdout, "\nTime = %.2f\n", t);

  free (arr_a);
  free (arr_b);
  free (arr_c);
  return 0;
}
