#include "solve.h"
#include "io_status.h"

io_status
t1_solve (const char *filename, const int k, int &r)
{
  const int len_int = sizeof (int) << 3;
  int i, n, len_plot = 0, dist = 0, min = -1;

  FILE *fp = fopen (filename, "r");
  if (!fp)
    return io_status::open;

  while (fscanf (fp, "%d", &n) == 1)
    {
      /* Ищем первый не нулевой бит, пропускаем ведущие нули.  */
      for (i = len_int - 1; ((n >> i) & 1) != 1 && i >= 0; i--)
        ;
      for (; i >= 0; i--)
        {
          if (((n >> i) & 1) == 1)
            len_plot++;
          else
            {
              if (len_plot >= k)
                {
                  if (min * dist < 0 || dist < min)
                    min = dist;
                  dist = 1;
                }
              else if (dist)
                dist += len_plot + 1;
              len_plot = 0;
            }
        }
    }

  if (len_plot >= k && (min * dist < 0 || dist < min))
        min = dist;

  r = min;

  fclose (fp);

  return io_status::success;
}
