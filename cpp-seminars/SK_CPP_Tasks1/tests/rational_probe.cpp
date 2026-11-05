#include "../01Ex/rational.h"

#include <cinttypes>
#include <cstdio>
#include <cstring>

int
main ()
{
  char kind;

  while (scanf (" %c", &kind) == 1)
    {
      rational value;

      if (kind == 'f')
        {
          uint32_t bits;
          float input;

          if (scanf ("%" SCNx32, &bits) != 1)
            return 1;

          memcpy (&input, &bits, sizeof (input));
          value.from_float (input);
        }
      else if (kind == 'r')
        {
          int64_t num, denom;

          if (scanf ("%" SCNd64 " %" SCNd64, &num, &denom) != 2)
            return 1;

          value.from_num_denom (num, denom);
        }
      else
        return 1;

      int64_t num, denom;
      uint32_t bits;
      float output = value.to_float ();

      value.to_num_denom (num, denom);
      memcpy (&bits, &output, sizeof (bits));
      printf ("%" PRId64 " %" PRId64 " %08" PRIx32 "\n", num, denom, bits);
    }

  return 0;
}
