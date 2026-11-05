#ifndef IO_STATUS_H
#define IO_STATUS_H

#define LEN 1234

typedef enum io_status_
{
  SUCCESS = 0,
  ERROR_OPEN = -1,
  ERROR_READ = -2,
  ERROR_PATTERN = -3,
  ERROR_WRITE = -4
} io_status;

#endif /* IO_STATUS_H */
