#include "solve.h"

#include <math.h>
#include <float.h>

double t4_solve (
		double (*f) (double), 
		double a, double b,
		int n	
) {
	const double h = (b - a) / n;
	double x = a;
	double sum = (f(a) + f(b)) * 0.5;

	if (h < NUM_FPE)
		return DBL_MAX;
	
	for (int i = 1; i < (n - 1); ++i)
	{
		x += h;
		sum += f(x);
	}

	return h * sum;
}

