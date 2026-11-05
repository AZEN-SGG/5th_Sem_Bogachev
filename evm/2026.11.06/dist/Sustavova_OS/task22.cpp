#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "thread_func.h"//prototipes of solves
#include "matrix_io.h"// named constantes
#include "io_status.h"
#include "mistake.h"
#include <pthread.h>
#include "Args.h"


int main (int argc, char *argv[])
{
  double *a = nullptr;
  double *b = nullptr;

  Args *ap = nullptr;
  pthread_t *tid = nullptr;
  int n = 0, m = 0, p = 0, r = 0, s = 0, task = 22;
  const char *name = nullptr;
  if (!((argc == 6 || argc == 7) &&
     sscanf(argv[1], "%d", &n) == 1 && n >= 0 &&
     sscanf(argv[2], "%d", &m) == 1 && m >= 0 &&
     sscanf(argv[3], "%d", &p) == 1 && p >= 0 &&
     sscanf(argv[4], "%d", &r) == 1 && r >= 0 &&
     sscanf(argv[5], "%d", &s) == 1 && s >= 0 && s <= 4) || 
      (s == 0 && argc != 7) || (s > 0 && argc != 6))
    {
      printf("Usage: %s n m p r s [filename]\n", argv[0]);
      return 1;
    }
  // Разбор аргументов командной строки.
  int blok = (n % m == 0)? n / m : n / m + 1;
  if (p > blok) p = blok; 
  ap = new Args[p];
  tid = new pthread_t[p];
  if (!ap)
  {
    printf("Memory error\n");
    return -1;
  // Обработка ошибки выделения памяти.
  }
  tid = new pthread_t[p];
  if (!tid)
  {
    printf("Memory error\n");
    delete[] ap;
    return -1;
  }
  a = new double[n * n];
  if (!a)
  {
    printf("Memory error\n");
    delete ap;
    delete[] tid;
    return -1;
  }
  b = new double[n * n];
  if (!b)
  {
    printf("Memory error\n");
    delete[] ap;
    delete[] tid;
    delete[] a;
    return -1;
  }
  for (int u = 0; u < p; ++u)
  {
        ap[u].a = a;
        ap[u].b = b;
        ap[u].name = name;
        ap[u].n = n;
        ap[u].m = m;
        ap[u].p = p;
        ap[u].k = u;
        ap[u].s = s;
        ap[u].r = r;
  }
  for (int u = 1; u < p; ++u)
  {
    if (pthread_create (tid + u, nullptr, thread_func, ap + u))
    {
      printf("error of pthread_create\n");
      delete[] ap;
      delete[] tid;
      delete[] a;
      delete[] b;
      return -1;
    // Обработка ошибки создания потока.
    }
  }
  thread_func (ap + 0);
  for (int u = 1; u < p; ++u) pthread_join (tid[u], nullptr);
  // Печать результата и освобождение памяти.
  if (ap[0].error)
  {
    printf("Not appliable\n");
    printf ("%s : Task = %d Res1 = %e Res2 = %e T1 = %.2f T2 = %.2f S = %d N = %d M = %d\n", argv[0], task, -1., -1., ap[0].t_full, ap[0].t_2_full, s, n, m);
    delete[] ap;
    delete[] tid;
    delete[] a;
    delete[] b;
    return 0;

  }
  printf("Resalt matrix A^(-1):\n");
  print_matrix(b, n, n, r);
  printf ("%s : Task = %d Res1 = %e Res2 = %e T1 = %.2f T2 = %.2f S = %d N = %d M = %d\n", argv[0], task, ap[0].r1, ap[0].r2, ap[0].t_full, ap[0].t_2_full, s, n, m);
  
  delete[] ap;
  delete[] tid;
  delete[] a;
  delete[] b;
  return 0;
}