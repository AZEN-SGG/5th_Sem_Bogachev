#include "solve.h"

io_status
t3_solve (const char *f_in, const char *f_out, const char *s, const char *t,
          int *res)
{
  if (s == NULL)
    return ERROR_PATTERN;
  else
    {
      size_t len_s = strlen (s);
      io_status status;
      char s1[LEN];
      char s2[LEN];

      memset (s1, 0, len_s + 1);
      memset (s2, 0, len_s + 1);

      status = process_s (s, s1, s2, len_s);
      if (status != SUCCESS)
        return status;
      else
        {
          return process_file (f_in, f_out, s1, s2, t, res);
        }
    }
}
