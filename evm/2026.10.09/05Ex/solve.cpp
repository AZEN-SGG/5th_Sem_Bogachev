#include "solve.h"

comp_status
t5_solve (const size_t n, const size_t m,
    double *a, double *b, int *d, double *work)
{
  double norm = get_matrix_norm (n, a);
  if (norm <= 0)
    return comp_status::ERROR_SINGULAR;

  return cholesky_method (n, m, a, b, d, work, std::sqrt (norm));
}
