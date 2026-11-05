#include "solve.h"

int
t4_solve (const char *f_in, const char *f_out, const char *s)
{
  size_t len_s;
  io_status status;
  char s1[LEN], s2[LEN];

  if (!s)
    return ERROR_PATTERN;

  len_s = strlen (s);
  memset (s1, 0, LEN);
  memset (s2, 0, LEN);

  status = process_s (s, s1, s2, len_s);
  if (status != SUCCESS)
    return status;

  return file_match (f_in, f_out, s1, s2);
}

io_status
process_s (const char *s, char *s1, char *s2, const size_t len_s)
{
  size_t i;
  int j;
  for (i = 0, j = 0; i < len_s; ++i)
    {
      s1[j] = '0';
      if (s[i] == '\\')
        {
          if (i + 1 == len_s)
            return ERROR_PATTERN;
          if (s[++i] == '>')
            s1[j] = '1';
        }
      s2[j++] = s[i];
    }
  s2[j] = '\0';
  return SUCCESS;
}

bool
process_string (const char *buf, const char *s1, const char *s2,
                const size_t len_s, const size_t len_buf)
{
  size_t i;
  for (i = len_buf; i < i + 2; --i)
    {
      size_t pos = i;
      size_t j;

      for (j = 0; j < len_s; ++j)
        {
          if (s1[j] == '1')
            {
              if (pos == 0 || is_space (buf[pos - 1])
                  || (pos < len_buf && !is_space (buf[pos])))
                break;
            }
          else
            {
              if (pos == len_buf || buf[pos] != s2[j])
                break;
              ++pos;
            }
        }
      if (j == len_s)
        return true;
    }
  return false;
}

bool
is_space (char symbol)
{
  return symbol == ' ' || symbol == '\t';
}

int
file_match (const char *f_in, const char *f_out, const char *s1,
            const char *s2)
{
  FILE *fp = fopen (f_in, "r");
  if (!fp)
    return ERROR_OPEN;
  else
    {
      char buf[LEN] = { 0 };
      size_t len_s = strlen (s2);
      FILE *fw = fopen (f_out, "w");
      int i;
      int status = SUCCESS;

      if (!fw)
        {
          fclose (fp);
          return ERROR_OPEN;
        }
      for (i = 0; fgets (buf, sizeof (buf), fp);)
        {
          size_t len_buf = strlen (buf);
          bool is_approach;

          if (len_buf > 0 && buf[len_buf - 1] == '\n')
            {
              buf[--len_buf] = '\0';
              if (len_buf > 0 && buf[len_buf - 1] == '\r')
                buf[--len_buf] = '\0';
            }
          is_approach = process_string (buf, s1, s2, len_s, len_buf);
          if (is_approach)
            {
              if (fprintf (fw, "%s\n", buf) < 0)
                {
                  status = ERROR_WRITE;
                  break;
                }
              i++;
            }
        }
      if (ferror (fp) && status == SUCCESS)
        status = ERROR_READ;
      if (fclose (fw) != 0 && status == SUCCESS)
        status = ERROR_WRITE;
      if (fclose (fp) != 0 && status == SUCCESS)
        status = ERROR_READ;
      return status == SUCCESS ? i : status;
    }
}
