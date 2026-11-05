#include "date_type.h"

#include <climits>

static int64_t
divide_floor (int64_t value, int64_t divisor)
{
  int64_t result = value / divisor;

  if (value % divisor < 0)
    --result;

  return result;
}

static int64_t
days_before_year (int64_t year)
{
  int64_t previous = year - 1;

  return previous * 365 + divide_floor (previous, 4)
         - divide_floor (previous, 100) + divide_floor (previous, 400);
}

static int
days_in_month (int month, int64_t year)
{
  static const int days[] = { 31, 28, 31, 30, 31, 30,
                             31, 31, 30, 31, 30, 31 };
  bool leap = year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);

  return days[month - 1] + (month == 2 && leap);
}

static bool
valid_date (int dd, int mm, int yy)
{
  return mm >= 1 && mm <= 12 && dd >= 1 && dd <= days_in_month (mm, yy);
}

static bool
valid_time (int hh, int mm, int ss)
{
  return hh >= 0 && hh <= 23 && mm >= 0 && mm <= 59 && ss >= 0 && ss <= 59;
}

date_type::date_type (int64_t unix_epoch_sec)
{
  m_unix_epoch_sec = unix_epoch_sec;
}

date_type::date_type (int d, int m, int y, int hh, int mm, int ss)
{
  if (!valid_date (d, m, y))
    {
      fprintf (stderr, "Invalid date\n");
      return;
    }

  if (!valid_time (hh, mm, ss))
    {
      fprintf (stderr, "Invalid time\n");
      return;
    }

  set_date (d, m, y);
  set_time (hh, mm, ss);
}

void
date_type::set_date (int dd, int mm, int yy)
{
  if (!valid_date (dd, mm, yy))
    {
      fprintf (stderr, "Invalid date\n");
      return;
    }

  int64_t days = days_before_year (yy) - days_before_year (1970);
  int64_t time = m_unix_epoch_sec % 86400;

  for (int month = 1; month < mm; ++month)
    days += days_in_month (month, yy);

  days += dd - 1;
  if (time < 0)
    time += 86400;

  m_unix_epoch_sec = days * 86400 + time;
}

void
date_type::get_date (int &dd, int &mm, int &yy) const
{
  int64_t days = divide_floor (m_unix_epoch_sec, 86400)
                 + days_before_year (1970);
  int64_t era = divide_floor (days, 146097);
  int64_t first = era * 400 + 1;
  int64_t last = first + 399;

  while (first < last)
    {
      int64_t middle = first + (last - first + 1) / 2;

      if (days_before_year (middle) <= days)
        first = middle;
      else
        last = middle - 1;
    }

  if (first < INT_MIN || first > INT_MAX)
    {
      fprintf (stderr, "Year is out of range\n");
      return;
    }

  days -= days_before_year (first);
  int month = 1;

  while (days >= days_in_month (month, first))
    {
      days -= days_in_month (month, first);
      ++month;
    }

  dd = (int)days + 1;
  mm = month;
  yy = (int)first;
}

void
date_type::set_time (int hh, int mm, int ss)
{
  if (!valid_time (hh, mm, ss))
    {
      fprintf (stderr, "Invalid time\n");
      return;
    }

  int64_t time = m_unix_epoch_sec % 86400;

  if (time < 0)
    time += 86400;

  m_unix_epoch_sec += hh * 3600 + mm * 60 + ss - time;
}

void
date_type::get_time (int &hh, int &mm, int &ss) const
{
  int64_t time = m_unix_epoch_sec % 86400;

  if (time < 0)
    time += 86400;

  hh = (int)(time / 3600);
  mm = (int)(time % 3600 / 60);
  ss = (int)(time % 60);
}
