#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "solve.h"//prototipes of solves
#include "matrix_io.h"// named constantes
#include "io_status.h"
#include "mistake.h"
#include "reduce_sum.h"
#include "Args.h"
#include <pthread.h>
#include <sys/sysinfo.h>
#include <cstring>
#include <sys/time.h>   
#include <sys/resource.h> 



double get_full_time();
double get_cpu_time();

void * thread_func (void *ptr)
{
  Args *ap = (Args *)ptr;
  double *a = ap->a;
  double *b = ap->b;
  const char *name = ap->name;
  int n = ap->n;
  int m = ap->m;
  int p = ap->p;
  int k = ap->k;
  int r = ap->r;
  int s = ap->s;
  int i;
  cpu_set_t cpu;
  CPU_ZERO (&cpu);
  int n_cpus = get_nprocs ();
  int cpuid = n_cpus - 1 - (k % n_cpus);
  CPU_SET (cpuid, &cpu);
  pthread_t tid = pthread_self ();
  pthread_setaffinity_np (tid, sizeof (cpu), &cpu);
  for (int i = k * m; i < n; i += p * m)
  {
    int h = (i + m < n ? m : n - i);
    memset (a + i * n, 0, h * n * sizeof (double));
    memset (b + i * n, 0, h * m * sizeof (double));
  }
  double res = 0;
  if (name != nullptr)
  {
    if (k == 0) 
    {
      io_status errr = read_matrix (a, n, n, name);
      if (errr == io_status::success)
      {
        print_matrix(a, n, n, r);
      }
      else res = -1.;
    }
    reduce_sum (p, &res, 1);
    if (res < 0)
    {
      if (k == 0) printf("Error read\n");
      return nullptr;
    }
  }
  else
  {
    init_matrix (a, n, m, s, k, p);
    if (k == 0) print_matrix(a, n, n, r);
  }
  init_matrix_b (b, n, m, k, p);
  res = 0;
  double *ccos, *ssin, *A_1, *A_2;
  ccos = new double [m * m];
  if (ccos)
  {
    ssin = new double [m * m];
    if (ssin)
    {
      A_1 = new double [m * m];
      if (A_1)
      {
        A_2 = new double [m * m];
        if (!A_2)
        {
          printf("Memory error\n");
          delete[] ccos;
          delete[] ssin;
          delete[] A_1;
          res = -1;
        }
      }
      else
      {
        delete[] ccos;
        delete[] ssin;
        res = -1.;
      }
    }
    else
    {
      delete[] ccos;
      res = -1;
    }
  }
  reduce_sum (p, &res, 1);
  if (res < 0)
  {
    if (k == 0) printf("Memory error\n");
    return nullptr;
  }
  double normA = norm(a, n, p, k);
  for (i = 0; i < p; i++) A_1[i] = (i != k)? 0. : normA;
  reduce_sum(p, A_1, p);
  for (i = 0; i < p; i++) if (normA < A_1[i]) normA = A_1[i];
  ap->Norma = normA;
  //printf("%e = norma\n", normA);
  double t_full = get_full_time ();
  double t = get_cpu_time ();
  int error = solve (a, b, n, m, k, p, A_1, A_2, ccos, ssin, normA);
  t = get_cpu_time () - t;
  t_full = get_full_time () - t_full;
  ap->t = t;
  ap->t_full = t_full;
  ap->error = error;
  if (error)
  {
    delete[] ccos;
    delete[] ssin;
    delete[] A_1;
    delete[] A_2;
    reduce_sum (p);
    return nullptr;
  }

  if (name != nullptr)
  {
    if (k == 0) 
    {
      read_matrix (a, n, n, name);
    }
    reduce_sum (p, &res, 1);
    if (res < 0)
    {
      if (k == 0) printf("Error read\n");
      return nullptr;
    }
  }
  else
  {
    init_matrix (a, n, m, s, k, p);
  }
  reduce_sum(p);


  t_full = get_full_time ();
  t = get_cpu_time ();
  for (i = 0; i < p; i++)
  {
    if (i != k)
    {
      A_1[i] = 0;
      A_2[i] = 0;
    }
    else 
    {
      A_1[i] = mistake(a, b, ccos, ssin, n, m, p, k);
      A_2[i] = mistake(b, a, ccos, ssin, n, m, p, k);
    }
  }
  // for (i = 0; i < p; i++) printf("%e ", A_1[i]);
  // printf(" - r1 potok %d\n", k);
  // for (i = 0; i < p; i++) printf("%e ", A_2[i]);
  // printf(" - r2 potok %d\n", k);
  reduce_sum(p, A_1, p);
  reduce_sum(p, A_2, p);
  double r1 = -1, r2 = -1;
  for (i = 0; i < p; i++)
  {
    if (A_1[i] > r1) r1 = A_1[i];
    if (A_2[i] > r2) r2 = A_2[i];
  }
  t = get_cpu_time () - t;
  t_full = get_full_time () - t_full;
  ap->r1 = r1;
  ap->r2 = r2;
  ap->t_2 = t;
  ap->t_2_full = t_full;
  delete[] ccos;
  delete[] ssin;
  delete[] A_1;
  delete[] A_2;
  reduce_sum (p);
  return nullptr;
}


double get_full_time()
{
  struct timeval buf;
  gettimeofday(&buf, 0);
  return buf.tv_sec + buf.tv_usec / 1.e6;
}

double get_cpu_time()
{
  struct rusage buf;
  getrusage(RUSAGE_THREAD, &buf);
  return buf.ru_utime.tv_sec + buf.ru_utime.tv_usec / 1.e6;
}
