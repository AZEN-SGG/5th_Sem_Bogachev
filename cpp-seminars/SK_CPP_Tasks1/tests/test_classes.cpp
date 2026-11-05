#include "../03Ex/block.h"
#include "../02Ex/date_type.h"
#include "../01Ex/rational.h"

#include <climits>
#include <cmath>
#include <cstdio>
#include <ctime>
#include <limits>
#include <type_traits>

static_assert (!std::is_copy_constructible<block>::value, "Block owns memory");
static_assert (!std::is_copy_assignable<block>::value, "Block owns memory");

static int errors = 0;
static int checks = 0;

static void
check (bool value, const char *name)
{
  checks++;
  if (!value)
    {
      fprintf (stderr, "Failed: %s\n", name);
      errors++;
    }
}

static bool
equal (double a, double b)
{
  return fabs (a - b) < 1.e-9;
}

static void
check_fraction (const rational &value, int64_t num, int64_t denom,
                const char *name)
{
  int64_t actual_num, actual_denom;

  value.to_num_denom (actual_num, actual_denom);
  check (actual_num == num && actual_denom == denom, name);
}

static void
test_rational ()
{
  check_fraction (rational (), 0, 1, "default rational");
  check_fraction (rational (6, -8), -3, 4, "normalize rational");
  check_fraction (rational (0, -8), 0, 1, "normalize zero");
  check_fraction (rational (INT64_MIN, INT64_MIN), 1, 1, "minimum ratio");
  check_fraction (rational (INT64_MIN, -1), INT64_MIN, -1,
                  "maximum magnitude");
  check_fraction (rational (0.75f), 3, 4, "exact binary fraction");
  check_fraction (rational (0.1f), 13421773, 134217728, "exact float value");
  check_fraction (rational (-0.1f), -13421773, 134217728,
                  "negative float value");
  check_fraction (rational (std::ldexp (1.f, -63)), -1, INT64_MIN,
                  "maximum denominator");
  check_fraction (rational (std::ldexp (1.f, -64)), 0, 1,
                  "nearest fraction tie");
  check_fraction (rational (std::nextafter (std::ldexp (1.f, -64), 1.f)),
                  -1, INT64_MIN, "nearest fraction above tie");
  check_fraction (rational (std::numeric_limits<float>::denorm_min ()),
                  0, 1, "smallest float");
  check_fraction (rational (std::numeric_limits<float>::max ()),
                  INT64_MIN, -1, "largest float");
  check_fraction (rational (-std::numeric_limits<float>::max ()),
                  INT64_MIN, 1, "smallest bounded rational");

  rational value (1, 2);
  rational third (1, 3);

  check (&value.add (third) == &value, "add returns current object");
  check_fraction (value, 5, 6, "rational addition");
  check (&value.subtract (third) == &value, "subtract returns current object");
  check_fraction (value, 1, 2, "rational subtraction");
  check (&value.multiply (third) == &value, "multiply returns current object");
  check_fraction (value, 1, 6, "rational multiplication");
  check (&value.divide (third) == &value, "divide returns current object");
  check_fraction (value, 1, 2, "rational division");

  value.add (value);
  check_fraction (value, 1, 1, "self addition");
  value.subtract (value);
  check_fraction (value, 0, 1, "self subtraction");
  value.from_num_denom (2, 3);
  value.multiply (value);
  check_fraction (value, 4, 9, "self multiplication");
  value.divide (value);
  check_fraction (value, 1, 1, "self division");
  value.from_num_denom (INT64_MAX, 3);
  value.multiply (rational (3, INT64_MAX));
  check_fraction (value, 1, 1, "cross cancellation");
  value.from_num_denom (1, INT64_MAX);
  value.add (value);
  check_fraction (value, 2, INT64_MAX, "common denominator");
  value.from_num_denom (1, INT64_MIN);
  value.add (value);
  check_fraction (value, -1, (int64_t)1 << 62, "minimum denominator addition");

  int64_t denom = ((int64_t)1 << 62) - 1;
  int64_t num = denom + ((int64_t)1 << 38);

  check (rational (num, denom).to_float () > 1.f,
         "round above float midpoint");
  check (!(rational (num - 1, denom).to_float () > 1.f),
         "round below float midpoint");
  check (!(rational (16777217, 16777216).to_float () > 1.f),
         "round midpoint to even");
  check (equal (rational (-3, 4).to_float (), -0.75), "negative to float");

  float infinity = std::numeric_limits<float>::infinity ();
  rational positive (1, 1);
  rational negative (-1, 1);
  rational zero;

  positive.divide (zero);
  negative.divide (zero);
  check (std::isinf (positive.to_float ()) && positive.to_float () > 0,
         "positive division by zero");
  check (std::isinf (negative.to_float ()) && negative.to_float () < 0,
         "negative division by zero");
  check_fraction (positive, 1, 0, "infinity representation");
  zero.divide (zero);
  check (std::isnan (zero.to_float ()), "zero divided by zero");
  positive.add (rational (infinity));
  check (std::isinf (positive.to_float ()), "add infinities");
  positive.subtract (positive);
  check (std::isnan (positive.to_float ()), "subtract equal infinities");
  positive.from_float (infinity);
  positive.multiply (rational ());
  check (std::isnan (positive.to_float ()), "zero times infinity");
  value.from_num_denom (1, 1);
  value.divide (rational (infinity));
  check_fraction (value, 0, 1, "finite divided by infinity");
  value.from_float (std::numeric_limits<float>::quiet_NaN ());
  check (std::isnan (value.to_float ()), "nan conversion");
}

static void
test_block_memory ()
{
  double *external = new double[4]{ 1., 2., 3., 4. };

  {
    block a;
    double *start = a.allocate_block (4);
    check (start != nullptr, "allocate_block");
    if (!start)
      {
        delete[] external;
        return;
      }

    for (size_t i = 0; i < 4; ++i)
      check (equal (start[i], 0.), "zero initialization");

    start[0] = 5.;
    a.link_block (start, 4);
    check (equal (start[0], 5.), "link own pointer");

    block b (external, 4);
    a.swap_block (b);
    a.free_block ();
    check (equal (external[0], 1.), "swap external ownership");
    b.swap_block (b);
    b.free_block ();
    b.free_block ();

    a.link_block (external, 4);
    start = a.allocate_block (2);
    check (start != nullptr, "replace external block");
    check (equal (external[3], 4.), "external block survives allocate");
    if (start)
      {
        check (equal (start[0], 0.), "replacement initialization");
        start = a.allocate_block (3);
        check (start != nullptr, "replace own block");
      }

    a.allocate_block (0);
    a.free_block ();
  }

  {
    block a (external, 4);
  }
  check (equal (external[2], 3.), "external block survives destructor");
  delete[] external;

  {
    block a (4);
    block b;
    a.swap_block (b);
  }
}

static void
test_block_operations ()
{
  block a;
  block b;
  block empty;
  double *left = a.allocate_block (3);
  double *right = b.allocate_block (1);

  check (left && right, "operation allocation");
  if (!left || !right)
    return;

  left[0] = 1.;
  left[1] = 2.;
  left[2] = 100.;
  right[0] = 10.;

  check (equal (a.add_block (b), 100.), "maximum includes tail");
  check (equal (left[0], 11.), "add common elements");
  check (equal (left[1], 2.), "add preserves tail");
  check (equal (a.sub_block (b), 1.), "subtract common elements");

  right[0] = -20.;
  left[2] = -100.;
  check (equal (a.sub_block (b), -100.), "minimum includes tail");
  check (equal (left[0], 21.), "subtract negative element");

  right[0] = -2.;
  left[0] = -1.;
  left[1] = -4.;
  check (equal (a.add_block (b), -3.), "negative maximum");
  check (equal (b.add_block (a), -5.), "short destination addition");
  check (equal (a.add_block (empty), -3.), "add empty source");
  check (equal (empty.add_block (a), 0.), "add to empty destination");
  check (equal (empty.sub_block (a), 0.), "subtract from empty destination");

  b.copy_to (a);
  check (equal (left[0], -5.), "copy to larger destination");
  check (equal (left[1], -4.), "copy preserves destination tail");
  left[0] = 7.;
  b.copy_from (a);
  check (equal (right[0], 7.), "copy from larger source");
  a.copy_to (a);
  a.copy_from (a);
  check (equal (left[0], 7.), "self copy");
  empty.copy_to (a);
  a.copy_from (empty);
  check (equal (left[0], 7.), "copy empty source");
  a.copy_to (empty);
  empty.copy_from (a);
  check (equal (a.add_block (a), 14.), "self addition");
  check (equal (a.sub_block (a), 0.), "self subtraction");

  left[0] = 5.;
  left[1] = 6.;
  left[2] = 7.;

  {
    block view (left + 1, 2);

    a.copy_to (view);
    check (equal (left[1], 5.) && equal (left[2], 6.),
           "overlapping copy forward");
    a.copy_from (view);
    check (equal (left[0], 5.) && equal (left[1], 6.),
           "overlapping copy backward");
  }

  a.link_block (nullptr, 3);
  check (equal (left[0], 5.), "invalid link preserves block");
  check (a.allocate_block (SIZE_MAX) == nullptr, "reject invalid block size");
  check (equal (left[0], 5.), "invalid size preserves block");
}

static void
check_date (const date_type &date, int day, int month, int year,
            int hour, int minute, int second)
{
  int dd, mm, yy;
  int hh, min, ss;

  date.get_date (dd, mm, yy);
  date.get_time (hh, min, ss);
  check (dd == day && mm == month && yy == year, "calendar date");
  check (hh == hour && min == minute && ss == second, "time of day");
}

static void
test_date ()
{
  date_type date;

  check_date (date, 1, 1, 1970, 0, 0, 0);
  check_date (date_type (-1), 31, 12, 1969, 23, 59, 59);
  check_date (date_type (-86400), 31, 12, 1969, 0, 0, 0);
  check_date (date_type (-86401), 30, 12, 1969, 23, 59, 59);
  check_date (date_type (2147483647), 19, 1, 2038, 3, 14, 7);

  date_type leap (29, 2, 2000, 12, 34, 56);

  check (leap.get_walltime_sec () == 951827696, "known leap timestamp");
  leap.set_date (31, 12, 1969);
  check_date (leap, 31, 12, 1969, 12, 34, 56);
  leap.set_time (23, 59, 59);
  check (leap.get_walltime_sec () == -1, "replace time before epoch");
  leap.set_time (0, 0, 0);
  check (leap.get_walltime_sec () == -86400, "midnight before epoch");

  const int invalid_dates[][3] = { { 29, 2, 1900 }, { 29, 2, 2100 },
                                  { 31, 4, 2000 }, { 0, 1, 2000 },
                                  { 1, 0, 2000 }, { 1, 13, 2000 } };

  for (const auto &value : invalid_dates)
    {
      leap.set_date (value[0], value[1], value[2]);
      check (leap.get_walltime_sec () == -86400,
             "invalid date preserves value");
    }

  const int invalid_times[][3] = { { -1, 0, 0 }, { 24, 0, 0 },
                                  { 0, -1, 0 }, { 0, 60, 0 },
                                  { 0, 0, -1 }, { 0, 0, 60 } };

  for (const auto &value : invalid_times)
    {
      leap.set_time (value[0], value[1], value[2]);
      check (leap.get_walltime_sec () == -86400,
             "invalid time preserves value");
    }

  check (date_type (29, 2, 1900, 12, 34, 56).get_walltime_sec () == 0,
         "invalid constructor date preserves default value");
  check (date_type (1, 1, 2000, 24, 0, 0).get_walltime_sec () == 0,
         "invalid constructor time preserves default value");

  const int64_t outside_values[] = { INT64_MIN, INT64_MAX };

  for (int64_t value : outside_values)
    {
      int dd = 11, mm = 12, yy = 2000;
      date_type outside (value);

      outside.get_date (dd, mm, yy);
      check (dd == 11 && mm == 12 && yy == 2000,
             "invalid year preserves output arguments");
      outside.print ();
    }

  const int years[] = { INT_MIN, -400, -100, -1, 0, 1, 400, INT_MAX };

  for (int year : years)
    {
      date_type first (1, 1, year);
      date_type last (31, 12, year, 23, 59, 59);

      check_date (first, 1, 1, year, 0, 0, 0);
      check_date (last, 31, 12, year, 23, 59, 59);
    }
}

static void
test_date_reference ()
{
  for (int64_t value = -11676096000LL; value <= 13569465600LL;
       value += 864037)
    {
      time_t raw = (time_t)value;
      const tm *reference = gmtime (&raw);

      check (reference != nullptr, "reference date");
      if (!reference)
        continue;

      date_type date (value);

      check_date (date, reference->tm_mday, reference->tm_mon + 1,
                  reference->tm_year + 1900, reference->tm_hour,
                  reference->tm_min, reference->tm_sec);

      date_type restored (reference->tm_mday, reference->tm_mon + 1,
                          reference->tm_year + 1900, reference->tm_hour,
                          reference->tm_min, reference->tm_sec);

      check (restored.get_walltime_sec () == value, "restore timestamp");
    }
}

static void
test_print ()
{
  date_type ().print ();
  date_type (-1).print ();

  block data;
  double *start = data.allocate_block (25);

  check (start != nullptr, "print allocation");
  if (!start)
    return;

  for (size_t i = 0; i < 25; ++i)
    start[i] = (double)i + 1.;

  data.print_block ();
  fputc ('\n', stderr);
}

int
main ()
{
  test_rational ();
  test_block_memory ();
  test_block_operations ();
  test_date ();
  test_date_reference ();
  test_print ();
  printf ("Checks: %d, errors: %d\n", checks, errors);
  return errors ? 1 : 0;
}
