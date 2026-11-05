int solve(double *a, double *b, int n, int m, double *A_1, double *A_2, double *ccos, double *ssin, double norm);
void get_block (double *a, double *b, int n, int m, int si, int sj);
void set_block (double *a, double *b, int n, int m, int si, int sj);
void Cij (double *matrix, double cos, double sin, int w, int l, int k);
void action_block(double *matrix, double *ccos, double *ssin, int h, int h1);
void work_with_pair(double *A_1, double *A_2, double *ccos, double *ssin, int h, int v, int h1);
int gauss(double *A, double *B, int h, int h1, double norm);
void sub_multiply(double *a, double *b, double *c, int v, int h, int h1);
void CHECK(double *a, double *b, double *RES, double *VEC, int n);
double sgn(double x);
double norm (double *a, int n);


