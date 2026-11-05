#ifndef SOLVE_H
#define SOLVE_H

#include "io_status.h"

#include <cstdio>
#include <cstring>

int t1_solve (const char *f_in, const char *f_out, const char *s);
io_status process_s (const char *s, char *s1, char *s2, const size_t len_s);
bool process_string (const char *buf, const char *s1, const char *s2,
                     const size_t len_s, const size_t len_buf);

int file_match (const char *f_in, const char *f_out, const char *s1,
                const char *s2);

#endif /* SOLVE_H */
