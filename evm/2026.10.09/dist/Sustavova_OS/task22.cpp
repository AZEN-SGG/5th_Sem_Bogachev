#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "solve.h"//prototipes of solves
#include "matrix_io.h"// named constantes
#include "io_status.h"
#include "mistake.h"


int main(int argc, char const *argv[])
{
  int n, m, r, s, error = 0, task = 22;
  io_status ret = io_status::success;
  double r1;
  double r2;
  const char *name = 0;
  double *a = 0;
  double *b = 0;
  double *block1;
  double *block2;
  double *ccos;
  double *ssin;
  double norma;
  double t1, t2;

  if (!((argc == 6 || argc == 5) &&
     sscanf(argv[1], "%d", &n) == 1 && n >= 0 &&
     sscanf(argv[2], "%d", &m) == 1 &&
     sscanf(argv[3], "%d", &r) == 1 && r >= 0 &&
     sscanf(argv[4], "%d", &s) == 1 && s >= 0 && s <= 4) || 
      (s == 0 && argc != 6) || (s > 0 && argc != 5))
    {
      printf("Usage: %s n m r s [filename]\n", argv[0]);
      return 1;
    }
  if (s == 0) name = argv[5];
  a = new double[n * n];
  if (!a)
    {
      printf("Not enough memory!\n");
      return 2;
    }
  if (name)
    {
      ret = read_matrix(a, n, n, name);
      do
        {
          switch (ret)
            {
              case io_status::success:
                continue;
              case io_status::error_open:
                printf ("Can not open %s\n", name);
                break;
              case io_status::error_read:
                printf ("Can not read %s\n", name);
                break;
            }
          delete[] a;
          return 3;
        }
      while(0);
    }
  else
    {
      init_matrix(a, n, n, s);
    }
  b = new double[n * n];
  if (!b)
    {
      printf("Not enough memory!\n");
      delete[] a;
      return 2;
    }
  for (int i = 0, index = 0; i < n; i++, index += n)
    {
      for (int j = 0; j < n; j++)
        {
          if (i == j)
            {
              b[index + j] = 1;
            } 
          else
            {
              b[index + j] = 0;
            }
          
        }
    }
  block1 = new double[m * m];
  if (!block1)
    {
      printf("Not enough memory!\n");
      delete[] a;
      delete[] b;
      return 2;
    }
  block2 = new double[m * m];
  if (! block2)
    {
      printf("Not enough memory!\n");
      delete[] a;
      delete[] b;
      delete[] block1;
      return 2;
    }

  ccos = new double[m * m];
  if (!ccos)
    {
      printf("Not enough memory!\n");
      delete[] a;
      delete[] b;
      delete[] block1;
      delete[] block2;
      return 2;
    }
  ssin = new double[m * m];
  if (!ssin)
    {
      printf("Not enough memory!\n");
      delete[] a;
      delete[] b;
      delete[] block1;
      delete[] block2;
      delete[] ccos;
      return 2;
    }
  printf("Initial matrix A:\n");
  print_matrix(a, n, n, r);
  printf("\n");
  t1 = clock();
  norma = norm(a, n);
  //printf ("%e - NORMA\n", norma);
  error = solve(a, b, n, m, block1, block2, ccos, ssin, norma);
  t1 = (clock() - t1) / CLOCKS_PER_SEC;
  if (error < 0)
    {
      printf("The solution algorithm is not applicable to the input matrix A\n");
      printf ("%s : Task = %d Res1 = %e Res2 = %e T1 = %.2f T2 = %.2f S = %d N = %d M = %d\n", argv[0], task, -1., -1., t1, -1., s, n, m);
      delete[] a;
      delete[] b;
      delete[] block1;
      delete[] block2;
      delete[] ccos;
      delete[] ssin;
      return - 1;
    }
  
  if (name)
    {
      ret = read_matrix(a, n, n, name);
      do
        {
          switch (ret)
            {
              case io_status::success:
                continue;
              case io_status::error_open:
                printf ("Can not open %s\n", name);
                break;
              case io_status::error_read:
                printf ("Can not read %s\n", name);
                break;
            }
          delete[] a;
          delete[] b;
          delete[] block1;
          delete[] block2;
          delete[] ccos;
          delete[] ssin;
          return 3;
        }
      while(0);
    }
  else
    {
      init_matrix(a, n, n, s);
    }
  t2 = clock ();
  r1 = mistake (a, b, block2, block1, n, m);
  r2 = mistake (b, a, block2, block1, n, m);
  t2 = (clock () - t2) / CLOCKS_PER_SEC;
  printf("Resalt matrix A^(-1):\n");
  print_matrix(b, n, n, r);
  printf ("%s : Task = %d Res1 = %e Res2 = %e T1 = %.2f T2 = %.2f S = %d N = %d M = %d\n", argv[0], task, r1, r2, t1, t2, s, n, m);
  delete[] a;
  delete[] b;
  delete[] block1;
  delete[] block2;
  delete[] ccos;
  delete[] ssin;
  return 0;
  
}
