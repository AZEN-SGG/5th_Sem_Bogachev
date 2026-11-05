#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "matrix_io.h"// named constantes
#include "io_status.h"



io_status read_matrix(double *a, int n, int m, const char *name)
{
  int len = n * m;
  FILE *fp;
  //printf("Hello\n");
  if (!(fp = fopen(name, "r")))
    return io_status::error_open;
  for (int i = 0; i < len; i++)
  {
    if (fscanf(fp, "%lf", a + i) != 1)
    {
      fclose(fp);
      return io_status::error_read;
    }
  }
  fclose(fp);
  return io_status::success;
}



void init_matrix(double *a, int n, int m, int s, int k, int p)
{
  double (*function)(int, int, int, int);
  double (*f[])(int, int, int, int) = {f1, f2, f3, f4};
  function = f[s - 1];
  for (int i = k * m; i < n; i += p * m)
  {
    int h = (i + m < n ? m : n - i);
    for (int ii = 0; ii < h; ii++)
    {
      for (int jj = 0; jj < n; jj++)
      {
        a[(i + ii) * n + jj] = function(n, n, i + ii + 1, jj + 1);
      }
    }
  }
}


void init_matrix_b(double *b, int n, int m, int k, int p)
{
  for (int i = k * m; i < n; i += p * m)
  {
    int h = (i + m < n ? m : n - i);
    for (int ii = 0; ii < h; ii++)
    {
      for (int jj = 0; jj < n; jj++)
      {
        if ((ii + i) != jj) b[(i + ii) * n + jj] = 0;
        else b[(i + ii) * n + jj] = 1;
      }
    }
  }
}


void print_matrix(double *a, int n, int m, int p)
{
  int np = (n > p)? p : n;
  int mp = (m > p)? p : m;
  for (int i = 0; i < np; i++)
  {
    for (int j = 0; j < mp; j++)
      printf(" %10.3e", a[i * m + j]);
    printf("\n");
  }
}




double f1(int n, int m, int i, int j)
{
  return Maximum(n,m) - Maximum(i,j) + 1;
}



double f2(int n, int m, int i, int j)
{
  (void) n;
  (void) m;
  return Maximum(i,j);
}



double f3(int n, int m, int i, int j)
{
  (void) n;
  (void) m;
  return (i > j)? i - j : j - i;
}



double f4(int n, int m, int i, int j)
{
  (void) n;
  (void) m;
  return 1./(i + j - 1);
}

