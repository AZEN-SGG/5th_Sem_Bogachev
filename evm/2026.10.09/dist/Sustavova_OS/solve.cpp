#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "solve.h"//prototipes of solves
#include "matrix_io.h"// named constantes
#include "io_status.h"


double norm (double *a, int n)
{
    double res = 0, el;
    int i, j;
    for (j = 0; j < n; j++)
    {
        el = 0;
        for (i = 0; i < n; i++)
            el += fabs(a[i *n + j]);
        if (el > res) res = el;
    }
    return res;
}


int solve(double *a, double *b, int n, int m, double *A_1, double *A_2, double *ccos, double *ssin, double norm)
{
  int k = n / m;
  int l = n - k * m;
  double sum, sin, cos, x1, x2, el1, el2; 
  // double *RES = new double[n];
  // double *VEC = new double[n];
  int block = (l > 0)? k + 1 : k;
  int i, j, s, h, ii, jj, tt, h1, v1;
  // for (i = 0; i < n; i++)
  // {
  //   el1 = 0;
  //   for (j = 0; j < n; j += 2)
  //   {
  //     el1 += a[i * n + j];  
  //   }
  //   RES[i] = el1;
  // }
    
  for (s = 0; s < block; s++)//step of algoritm
    {
      //printf("NEW S = %d\n", s);
      get_block(a, A_1, n, m, s, s);
      //printf("WORK WITH A_%d %d\n", s, s);
      h = (s < k)? m : l;//square!
      for (ii = 0; ii < h; ii++)
        {
          x1 = A_1[ii * (h + 1)];
          for (jj = ii + 1; jj < h; jj++)
            {
              x2 = A_1[jj * h + ii];
              sum = sqrt (x1 * x1 + x2 * x2);
              //printf("sum = %lf\n", sum);
              if (sum < 3e-17 * norm)
                {
                  cos = 1; sin = 0;//stay as was
                }
              else
                {
                  cos = x1 / sum;
                  sin = - x2 / sum;
                }
              //printf("%d %d:cos = %lf; sin = %lf\n", ii, jj, cos, sin);
              ccos[h * ii - (((ii + 1) * ii)>> 1) + jj - ii - 1] = cos;
              ssin[h * ii - (((ii + 1) * ii)>> 1) + jj - ii - 1] = sin;
              A_1[ii * h + ii] = sum;
              x1 = sum;              
              A_1[jj * h + ii] = 0;
              for (tt = ii + 1; tt < h; tt++)
                {
                  el1 = A_1[ii * h + tt];
                  el2 = A_1[jj * h + tt];
                  A_1[ii * h + tt] = cos * el1 - sin * el2;
                  A_1[jj * h + tt] = sin * el1 + cos * el2;
                }
            }
        }
        set_block(a, A_1, n, m, s, s);
        //printf("\n");
      for (j = s + 1; j < block; j++)
        {
          get_block(a, A_2, n, m, s, j);
          //printf("WORK WITH A_%d %d\n", s, j);
          h1 = (j < k)? m : l;
          action_block(A_2, ccos, ssin, h, h1);
          set_block(a, A_2, n, m, s, j);
        }
      for (j = 0; j < block; j++)
        {
          get_block(b, A_2, n, m, s, j);
          //printf("WORK WITH B_%d %d\n", s, j);
          h1 = (j < k)? m : l;
          action_block(A_2, ccos, ssin, h, h1);
          set_block(b, A_2, n, m, s, j);
        }
      //WE CHANGED THE FIRST STRING
      // printf("FIRST STRING\n");
      // print_matrix(a, n, n, 7);
      // printf("\n");
      // print_matrix(b, n, n, 7);
      // printf("\n");
      // CHECK(a, b, RES, VEC, n);
      // printf("\n");


      for (i = s + 1; i < block; i++)//work with string i
        {
          get_block(a, A_1, n, m, s, s);
          get_block(a, A_2, n, m, i, s);
          // printf("WORK WITH PAIR A_%d %d and A_%d %d\n", s, s, i, s);
          // print_matrix(A_1, h, h, 9);
          v1 = (i < k)? m : l;
          //print_matrix(A_2, v1, h, 9);
          for (ii = 0; ii < h; ii++)
            {
              x1 = A_1[ii * h + ii];
              for (jj = 0; jj < v1; jj++)
              {
                x2 = A_2[jj * h + ii];
                sum = sqrt (x1 * x1 + x2 * x2);
                if (sum < 3e-17 * norm)
                {
                  cos = 1; sin = 0;//stay as was
                }
                else
                {
                  cos = x1 / sum;
                  sin = - x2 / sum;
                }
                //printf("%d %d:cos = %lf; sin = %lf\n", ii, jj, cos, sin);
                ccos[ii * v1 + jj] = cos;
                ssin[ii * v1 + jj] = sin;
                A_1[ii * h + ii] = sum;
                x1 = sum;//x1 = A_1[ii, ii]
                A_2[jj * h + ii] = 0;
                for (tt = ii + 1; tt < h; tt++)
                  {
                    el1 = A_1[ii * h + tt];
                    el2 = A_2[jj * h + tt];
                    A_1[ii * h + tt] = cos * el1 - sin * el2;
                    A_2[jj * h + tt] = sin * el1 + cos * el2;
                  }

              }
            }
          set_block(a, A_1, n, m, s, s);
          //set_block(a, A_2, n, m, i, s);!!!!!!!!!!!! донт нид!!!
          // printf("GOT\n");
          // print_matrix(A_1, h, h, 9);
          // print_matrix(A_2, v1, h, 9);


          //lerned what to do
          for (j = s + 1; j < block; j++)
            {
              get_block(a, A_1, n, m, s, j);
              get_block(a, A_2, n, m, i, j);
              //printf("WORK WITH PAIR A_%d %d and A_%d %d\n", s, j, i, j);
              h1 = (j < k)? m : l;
              work_with_pair(A_1, A_2, ccos, ssin, h, v1, h1);
              set_block(a, A_1, n, m, s, j);
              set_block(a, A_2, n, m, i, j);
              // print_matrix(A_1, h1, 9);
              // print_matrix(A_2, h1, 9);
              // printf("\n");
            }
            // printf("got\n");
            // print_matrix(a, n, 7);
            // printf("\n");
          for (j = 0; j < block; j++)
            {
              get_block(b, A_1, n, m, s, j);
              get_block(b, A_2, n, m, i, j);
              //printf("WORK WITH PAIR B_%d %d and B_%d %d\n", s, j, i, j);
              h1 = (j < k)? m : l;
              work_with_pair(A_1, A_2, ccos, ssin, h, v1, h1);
              set_block(b, A_1, n, m, s, j);
              set_block(b, A_2, n, m, i, j);
              // print_matrix(A_1, h1, 9);
              // print_matrix(A_2, h1, 9);
              // printf("\n");
            }
            // printf("I = %d STRING\n", i);
            // print_matrix(a, n, n, 7);
            // printf("\n");
            // print_matrix(b, n, n, 7);
            // printf("\n");
            // CHECK(a, b, RES, VEC, n);
            // printf("\n");
        }
    }//s
    //NOW WE BELIVE THAT A IS TRIANGLE
    // print_matrix(a, n, n, 9);
    // printf("\n");
    // print_matrix(b, n, n, 9);
    //GAUS
    //return gauss(a, b, n, n);
    double *C = ccos;
    for (s = block - 1; s >= 0; s--)
      {
        get_block(a, A_1, n, m, s, s);
        h = (s < k)? m : l;
        for (tt = 0; tt < block; tt++)
          {
              get_block(b, A_2, n, m, s, tt);
              h1 = (tt < k)? m : l;
              //A_k+1k+1 X = B_k+1tt
              if (gauss(A_1, A_2, h, h1, norm))
                return -1;
              set_block(b, A_2, n, m, s, tt);        
          }
          //substitute found roots

        for (j = 0; j < block; j++)
          {
            get_block(b, A_2, n, m, s, j);
            h1 = (j < k)? m : l;
            for (i = 0; i < s; i++)
              {
                get_block(a, A_1, n, m , i, s);
                get_block(b, C, n, m, i, j);
                sub_multiply(A_1, A_2, C, m, h, h1);
                set_block(b, C, n, m, i, j);
              }
          }
      }
  // delete[] RES;
  // delete[] VEC;
  return 0;
}


void get_block (double *a, double *b, int n, int m, int ii, int jj)//si, sj begin with zero
{
  int k = n / m, l = n - m * k;// n = k * m + l;
  int h = (ii < k)? m : l;
  int w = (jj < k)? m : l;
  for (int i = 0; i < h; i++)
    {
      for (int j = 0; j < w; j++)
        b[i * w + j] = a[(ii * m + i) * n + (jj * m + j)];
    }  
    //printf ("GETTT %d %d\n", ii, jj);
}

void set_block (double *a, double *b, int n, int m, int ii, int jj)//si, sj begin with 0
{
  int k = n / m, l = n - m * k;// n = k * m + l;
  int h = (ii < k)? m : l;
  int w = (jj < k)? m : l;
  for (int i = 0; i < h; i++)
    {
      for (int j = 0; j < w; j++)
        a[(ii * m + i) * n + (jj * m + j)] = b[i * w + j];
    }
    //printf ("SETTT %d %d\n", ii, jj);
}


void Cij (double *matrix, double cos, double sin, int w, int l, int k)//on ussual
{
  int j = 0;
  double xl, xk;
  for (j = 0; j < w; j++)
    {
      xl = matrix[l * w + j];
      xk = matrix[k * w + j];
      matrix[l * w + j] = cos * xl - sin * xk;
      matrix[k * w + j] = sin * xl + cos * xk;
    }
}


void action_block(double *matrix, double *ccos, double *ssin, int h, int h1)//h - wide of origin, h1 - wide of current
{
  int ii, jj;
  double cos, sin;
  for (ii = 0; ii < h; ii++)
    {
      for (jj = ii + 1; jj < h; jj++)
        {
          cos = ccos[h * ii - (((ii + 1) * ii)>> 1) + jj - ii - 1];
          sin = ssin[h * ii - (((ii + 1) * ii)>> 1) + jj - ii - 1];
          //printf("%d %d:cos = %lf; sin = %lf\n", ii, jj, cos, sin);
          Cij(matrix, cos, sin, h1, ii, jj);
        }
    }

}


void work_with_pair(double *A_1, double *A_2, double *ccos, double *ssin, int h, int v, int h1)
{
  int t, i, j;
  double cos, sin, el1, el2;
  // printf("WAS\n");
  // print_matrix(A_1, h, h1, 3);
  // print_matrix(A_2, v, h1, 3);

  for (i = 0; i < h; i++)
    {
      for (j = 0; j < v; j++)
        {
          cos = ccos[i * v + j];
          sin = ssin[i * v + j];
          //printf("%d %d:cos = %lf; sin = %lf\n", i, j, cos, sin);
          for (t = 0; t < h1; t++)
            {
              el1 = A_1[i * h1 + t];
              el2 = A_2[j * h1 + t];
              A_1[i * h1 + t] = cos * el1 - sin * el2;
              A_2[j * h1 + t] = sin * el1 + cos * el2; 
            }
            // printf("step\n");
            // print_matrix(A_1, h, h1, 3);
            // print_matrix(A_2, v, h1, 3);
        }
    }
  // printf("GOT\n");
  // print_matrix(A_1, h, h1, 3);
  // print_matrix(A_2, v, h1, 3);
}


int gauss(double *A, double *B, int h, int h1, double norm)
{
  int i, j, t;
  double sum;
  for (t = 0; t < h1; t++)
    {
      for (i = h - 1; i >= 0; i--)
        {
          //printf("%e - aaaaaa\n", fabs(A[i * (h + 1)]));
          if (fabs(A[i * (h + 1)]) < 3e-17 * norm) return -1;
          sum = 0;
          for (j = i + 1; j < h; j++)
            sum += A[i * h + j] * B[j * h1 + t];
          B[i * h1 + t] = (B[i * h1 + t] - sum) / A[i * (h + 1)];
        }
    }
  return 0;
}

void sub_multiply(double *a, double *b, double *c, int v, int h, int h1)
{
  int i, j, t;
  double el = 0;
  for (i = 0; i < v; i++)
    {
      for (j = 0; j < h1; j++)
        {
          el = 0;
          for (t = 0; t < h; t++)
            {
              el += a[i * h + t] * b[t * h1 + j];
            }
          c[i * h1 + j] -= el;
        }
    }
}


void CHECK(double *a, double *b, double *RES, double *VEC, int n)
{
  int i, j;
  double el1;
  for (i = 0; i < n; i++)
    {
      el1 = 0;
      for (j = 0; j < n; j++)
      {
        el1 += b[i * n + j] * RES[j];  
      }
      VEC[i] = el1;
    }
   for (i = 0; i < n; i++)
    {
      el1 = 0;
      for (j = 0; j < n; j += 2)
      {
        el1 += a[i * n + j];  
      }
      VEC[i] -= el1;
    }
    for (i = 0; i < n; i++)
      printf(" %10.3e", VEC[i]);
    printf("\n");


}

double sgn(double x)
{
  if (x >= 0) return 1;
  return -1;
}
