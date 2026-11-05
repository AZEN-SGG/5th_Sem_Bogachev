#define Maximum(A,B) ((A > B) ? A : B)
#include "io_status.h"



io_status read_matrix(double *a, int n, int m, const char *name); 
void print_matrix(double *a, int n, int m, int p);
void init_matrix(double *a, int n, int m, int k);
void init_matrix_b(double *b, double *a, int n);
double f1(int n, int m, int i, int j);
double f2(int n, int m, int i, int j);
double f3(int n, int m, int i, int j);
double f4(int n, int m, int i, int j);

