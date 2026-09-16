#include "solve.h"
#include <cstdio>

int
gcd (int a, int b)
{
  int temp;

  while (b)
    {
      temp = b;
      b = a % b;
      a = temp;
    }

  return a;
}

void
t5_solve (int k, double *arr, const int n)
{
  double last, temp;
  int gcd_v, inv_g, ind, next_ind;
  k = k % n;

  if (k == 0)
    return;

  gcd_v = gcd (n, k);
  inv_g = n / gcd_v;

  for (int i = 0 ; i < gcd_v ; ++i)
    {
      ind = i;
      last = arr[ind];
      for (int j = 0 ; j < inv_g ; ++j, ind = next_ind)
        {
          next_ind = (ind + inv_g) % n;
          temp = arr[next_ind];
          arr[next_ind] = last;
          last = temp;
        }
    }

  return;
}
