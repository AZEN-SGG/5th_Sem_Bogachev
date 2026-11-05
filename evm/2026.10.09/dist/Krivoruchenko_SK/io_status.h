#ifndef IO_STATUS_H
#define IO_STATUS_H

typedef enum _io_status
{
  SUCCESS,
  ERROR_OPEN,
  ERROR_READ,
  ERROR_MEM,
  ERROR_FORMAT,
  ERROR_SYMMETRY,
} io_status;

#endif
