#ifndef ARRAY_IO_H
#define ARRAY_IO_H

#include <cstdio>
#include <cstdlib>

void print_array (const int p, const int n, double *arr);
int read_file (const char *filename, const int n, double *&arr);
int gen_array (const int s, const int n, double *&arr);
double f (const int s, const int n, const int i);

#endif /* ARRAY_IO_H */
