#include "solve.h"
#include <cstdio>

int
t6_solve (int k, double *arr, const int n)
{
  double avg, sum = 0, sum_part = 0;
  int shift = 0, num = 0, num_part = 0;

  for (int i = 1; i < n; ++i)
    {
      if (arr[i] - arr[i - 1] < EPS)
        {
          sum_part += arr[i - 1];
          num_part++;
        }
      else if (num_part + 1 >= k)
        {
          sum_part += arr[i - 1];
          num_part++;

          sum += sum_part;
          num += num_part;

          sum_part = 0;
          num_part = 0;
        }
      else
        {
          sum_part = 0;
          num_part = 0;
        }
    }

  if (num_part + 1 >= k)
    {
      sum_part += arr[n - 1];
      num_part++;

      sum += sum_part;
      num += num_part;
    }

  if (!num)
    return n;

  avg = sum / num;

  for (int i = 0; i < n; ++i)
    {
      if (avg - arr[i] > EPS)
        shift++;
      else
        arr[i - shift] = arr[i];
    }

  return n - shift;
}
