#include "reduce_sum.h"
#include <pthread.h>
#include <stdio.h>
void reduce_sum(int p, double* a, int n)
{
  static pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
  static pthread_cond_t c_in = PTHREAD_COND_INITIALIZER;
  static pthread_cond_t c_out = PTHREAD_COND_INITIALIZER;
  static int t_in = 0;
  static int t_out = 0;
  static double* res = nullptr;
  int i;
  if (p <= 1)
    return;                                              
  pthread_mutex_lock(&m);
  if (res == nullptr) 
    res = a;
  else 
    for (i = 0; i < n; i++)
      res[i]+=a[i];
  t_in++;
  if (t_in >= p)
    { 
      t_out = 0;
      pthread_cond_broadcast(&c_in);
    }
  else
    {
      while(t_in < p) 
        pthread_cond_wait(&c_in, &m);
    }
  if (res != a)
    {
      for (i = 0; i < n; i++)
        a[i] = res[i];
    }
  t_out++;
  if (t_out >= p)
    {
      res = nullptr;
      t_in = 0; 
      //printf("POInt\n");
      pthread_cond_broadcast(&c_out);
    }
  else 
    {
      while(t_out < p)
        pthread_cond_wait(&c_out, &m);
    }
  pthread_mutex_unlock(&m);
}   