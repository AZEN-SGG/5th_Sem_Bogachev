#include <math.h>
#include <stdio.h>
double mistake (double *a, double *b, double *hight, double *pc, int n, int m, int p, int number)
{//pc - блок матрицы С = AB
  int ii, jj, k, l, bl, i, j, r, h = m, v, t, ah, h3, v3, s, q;
  double *pa, *pb, s00, s01, s02, s10, s11, s12, s20, s21, s22, sum, max = 0;
  //(void) p; (void) number;
  if (n > 11000) return 0;
  k = n / m;
  l = n - k * m;
  bl = (l != 0 ? k + 1 : k);
  for (j = number ; j < bl ; j+=p)
    {
      for (ii = 0; ii < m; ii++)
        hight[ii] = 0;
      for (i = 0 ; i < bl ; i++)
        {
          // Вычисляем размер блока v x h
          v = (i < k ? m : l);
          h = (j < k ? m : l);
          for (ii = 0; ii < m * m; pc[ii] = 0, ii++);
          if (i == j)//diagonal
          {
            for (ii = 0; ii < v; pc[ii * (h + 1)] = -1, ii++);
          } 
          for (s = 0 ; s < bl ; s++)
            {
              // A_is - v x ah, B_sj - ah x h
              ah = (s < k ? m : l);
              pa = a + i * n * m + s * m;
              pb = b + s * n * m + j * m;
              
              v3 = v % 3;
              h3 = h % 3;
              for (r = 0 ; r < v3 ; r++)
                {
                  for (t = 0 ; t < h3 ; t++)
                    {
                      sum = 0;
                      for (q = 0 ; q < ah ; q++)
                        sum += pa[r * n + q] * pb[q * n + t];
                      pc[r * h + t] += sum;
                    }
                  // Идём с шагом 3
                  for (t = h3; t < h ; t += 3)
                    {
                      // 3 блока суммы
                      s00 = 0;
                      s01 = 0;
                      s02 = 0;
                      for (q = 0 ; q < ah ; q++)
                        {
                          s00 += pa[r * n + q] * pb[q * n + t];
                          s01 += pa[r * n + q] * pb[q * n + t + 1];
                          s02 += pa[r * n + q] * pb[q * n + t + 2];
                        }
                      
                      pc[r * h + t] += s00;
                      pc[r * h + t + 1] += s01;
                      pc[r * h + t + 2] += s02;
                    }
                }
              
              for (r = v3; r < v ; r += 3)
                {
                  for (t = 0 ; t < h3 ; t++)
                      {
                        s00 = 0;
                        s10 = 0;
                        s20 = 0;
                        for (q = 0 ; q < ah ; q++)
                          {
                            s00 += pa[r * n + q] * pb[q * n + t];
                            s10 += pa[(r + 1) * n + q] * pb[q * n + t];
                            s20 += pa[(r + 2) * n + q] * pb[q * n + t];
                          }
                        
                        pc[r * h + t] += s00;
                        pc[(r + 1) * h + t] += s10;
                        pc[(r + 2) * h + t] += s20;
                      }
                    
                    for (t = h3; t < h ; t += 3)
                    {
                      // Ставим параллельно, а не единым блоком равенств, показывая компилятору, что они все независимы
                      s00 = 0;
                      s01 = 0;
                      s02 = 0;
                      s10 = 0;
                      s11 = 0;
                      s12 = 0;
                      s20 = 0;
                      s21 = 0;
                      s22 = 0;
                      
                      for (q = 0 ; q < ah ; q++)
                        {
                          s00 += pa[r * n + q] * pb[q * n + t];
                          s01 += pa[r * n + q] * pb[q * n + t + 1];
                          s02 += pa[r * n + q] * pb[q * n + t + 2];
                          s10 += pa[(r + 1) * n + q] * pb[q * n + t];
                          s11 += pa[(r + 1) * n + q] * pb[q * n + t + 1];
                          s12 += pa[(r + 1) * n + q] * pb[q * n + t + 2];
                          s20 += pa[(r + 2) * n + q] * pb[q * n + t];
                          s21 += pa[(r + 2) * n + q] * pb[q * n + t + 1];
                          s22 += pa[(r + 2) * n + q] * pb[q * n + t + 2];
                        }
                      pc[r * h + t] += s00;
                      pc[r * h + t + 1] += s01;
                      pc[r * h + t + 2] += s02;
                      pc[(r + 1) * h + t] += s10;
                      pc[(r + 1) * h + t + 1] += s11;
                      pc[(r + 1) * h + t + 2] += s12;
                      pc[(r + 2) * h + t] += s20;
                      pc[(r + 2) * h + t + 1] += s21;
                      pc[(r + 2) * h + t + 2] += s22;
                    } // t
                } // r
            } // s  
          for (jj = 0; jj < h; jj++)
            for (ii = 0; ii < v; ii++)
              hight[jj] += fabs(pc[ii * h + jj]);
        } // j
        for (jj = 0; jj < h; jj++)
          if (max < hight[jj]) max = hight[jj];
    } // i
    //printf("%e - maximum\n", max);
    return max;
}