#include "rational.h"

#include <cmath>
#include <limits>

static uint64_t
magnitude (int64_t value)
{
  return value < 0 ? 0 - (uint64_t)value : (uint64_t)value;
}

static uint64_t
gcd (uint64_t left, uint64_t right)
{
  while (right)
    {
      uint64_t remainder = left % right;

      left = right;
      right = remainder;
    }

  return left;
}

static int64_t
divide_common (int64_t value, uint64_t divisor)
{
  if (divisor > INT64_MAX)
    return value ? -1 : 0;

  return value / (int64_t)divisor;
}

static int64_t
signed_value (uint64_t value, bool negative)
{
  if (value > INT64_MAX)
    return INT64_MIN;

  return negative ? -(int64_t)value : (int64_t)value;
}

static void
set_fraction (uint64_t num, uint64_t denom, bool negative,
              int64_t &result_num, int64_t &result_denom)
{
  bool invert = denom > INT64_MAX || (!negative && num > INT64_MAX);

  result_num = signed_value (num, negative != invert);
  result_denom = signed_value (denom, invert);
}

static uint64_t
divide_power (int power, uint64_t divisor, uint64_t &remainder)
{
  uint64_t result = 1 / divisor;

  remainder = 1 % divisor;
  for (int i = 0; i < power; ++i)
    {
      remainder *= 2;
      uint64_t digit = remainder / divisor;

      remainder %= divisor;
      if (result > (UINT64_MAX - digit) / 2)
        result = UINT64_MAX;
      else
        result = result * 2 + digit;
    }

  return result;
}

rational::rational ()
{
  denominator = 1;
}

rational::rational (float value)
{
  from_float (value);
}

rational::rational (int64_t num, int64_t denom)
{
  from_num_denom (num, denom);
}

float
rational::to_float () const
{
  if (!denominator)
    {
      if (!numerator)
        return std::numeric_limits<float>::quiet_NaN ();

      float value = std::numeric_limits<float>::infinity ();

      return numerator < 0 ? -value : value;
    }

  if (!numerator)
    return 0.;

  uint64_t num = magnitude (numerator);
  uint64_t denom = magnitude (denominator);
  uint64_t whole = num / denom;
  uint64_t remainder = num % denom;
  uint64_t significand = whole;
  const int precision = std::numeric_limits<float>::digits;
  int exponent = 0;

  if (whole)
    {
      for (uint64_t value = whole; value > 1; value >>= 1)
        ++exponent;
    }
  else
    {
      do
        {
          remainder *= 2;
          --exponent;
        }
      while (remainder < denom);

      significand = 1;
      remainder -= denom;
    }

  if (exponent < precision)
    {
      int bits = whole ? exponent + 1 : 1;

      for (int i = bits; i < precision; ++i)
        {
          remainder *= 2;
          significand = significand * 2 + remainder / denom;
          remainder %= denom;
        }

      if (remainder > denom - remainder
          || (remainder == denom - remainder && (significand & 1)))
        ++significand;
    }
  else
    {
      int shift = exponent - precision + 1;
      uint64_t half = (uint64_t)1 << (shift - 1);
      uint64_t tail = whole & (((uint64_t)1 << shift) - 1);

      significand = whole >> shift;
      if (tail > half || (tail == half && (remainder || (significand & 1))))
        ++significand;
    }

  float value = std::ldexp ((float)significand, exponent - precision + 1);

  return (numerator < 0) != (denominator < 0) ? -value : value;
}

void
rational::from_float (float val)
{
  if (!std::isfinite (val))
    {
      numerator = std::isnan (val) ? 0 : (val < 0 ? -1 : 1);
      denominator = 0;
      return;
    }

  if (std::fpclassify (val) == FP_ZERO)
    {
      numerator = 0;
      denominator = 1;
      return;
    }

  bool negative = val < 0;
  double value = std::fabs ((double)val);
  const uint64_t limit = (uint64_t)INT64_MAX + 1;

  if (value >= std::ldexp (1., 63))
    {
      set_fraction (limit, 1, negative, numerator, denominator);
      return;
    }

  int exponent;
  double fraction = std::frexp (value, &exponent);
  const int precision = std::numeric_limits<float>::digits;
  uint64_t num = (uint64_t)std::ldexp (fraction, precision);
  int power = precision - exponent;

  while (!(num & 1))
    {
      num >>= 1;
      --power;
    }

  if (power <= 0)
    {
      set_fraction (num << -power, 1, negative, numerator, denominator);
      return;
    }

  uint64_t p0 = 1, q0 = 0;
  uint64_t p1 = power < 64 ? num >> power : 0;
  uint64_t q1 = 1;
  uint64_t divisor = power < 64 ? num - (p1 << power) : num;
  uint64_t remainder;
  uint64_t coefficient = divide_power (power, divisor, remainder);

  while (true)
    {
      uint64_t bound = (limit - q0) / q1;

      if (p1 && (limit - p0) / p1 < bound)
        bound = (limit - p0) / p1;

      if (coefficient > bound)
        {
          uint64_t p = p0 + bound * p1;
          uint64_t q = q0 + bound * q1;
          uint64_t unused;
          uint64_t middle = divide_power (power, 2 * divisor, unused);

          if (q > middle)
            {
              p1 = p;
              q1 = q;
            }

          break;
        }

      uint64_t p = p0 + coefficient * p1;
      uint64_t q = q0 + coefficient * q1;

      p0 = p1;
      q0 = q1;
      p1 = p;
      q1 = q;
      if (!remainder)
        break;

      uint64_t previous = divisor;

      divisor = remainder;
      coefficient = previous / divisor;
      remainder = previous % divisor;
    }

  set_fraction (p1, q1, negative, numerator, denominator);
}

rational &
rational::add (const rational &rhs)
{
  if (!denominator || !rhs.denominator)
    {
      from_float (to_float () + rhs.to_float ());
      return *this;
    }

  uint64_t divisor = gcd (magnitude (denominator),
                          magnitude (rhs.denominator));
  int64_t left = divide_common (denominator, divisor);
  int64_t right = divide_common (rhs.denominator, divisor);
  int64_t num = divisor > INT64_MAX ? numerator + rhs.numerator
                                  : numerator * right + rhs.numerator * left;
  int64_t denom = divisor > INT64_MAX ? denominator : left * rhs.denominator;

  from_num_denom (num, denom);
  return *this;
}

rational &
rational::subtract (const rational &rhs)
{
  if (!denominator || !rhs.denominator)
    {
      from_float (to_float () - rhs.to_float ());
      return *this;
    }

  uint64_t divisor = gcd (magnitude (denominator),
                          magnitude (rhs.denominator));
  int64_t left = divide_common (denominator, divisor);
  int64_t right = divide_common (rhs.denominator, divisor);
  int64_t num = divisor > INT64_MAX ? numerator - rhs.numerator
                                  : numerator * right - rhs.numerator * left;
  int64_t denom = divisor > INT64_MAX ? denominator : left * rhs.denominator;

  from_num_denom (num, denom);
  return *this;
}

rational &
rational::multiply (const rational &rhs)
{
  if (!denominator || !rhs.denominator)
    {
      from_float (to_float () * rhs.to_float ());
      return *this;
    }

  uint64_t first = gcd (magnitude (numerator), magnitude (rhs.denominator));
  uint64_t second = gcd (magnitude (rhs.numerator), magnitude (denominator));
  int64_t num = divide_common (numerator, first)
                * divide_common (rhs.numerator, second);
  int64_t denom = divide_common (denominator, second)
                  * divide_common (rhs.denominator, first);

  from_num_denom (num, denom);
  return *this;
}

rational &
rational::divide (const rational &rhs)
{
  if (!denominator || !rhs.denominator || !rhs.numerator)
    {
      from_float (to_float () / rhs.to_float ());
      return *this;
    }

  uint64_t first = gcd (magnitude (numerator), magnitude (rhs.numerator));
  uint64_t second = gcd (magnitude (denominator), magnitude (rhs.denominator));
  int64_t num = divide_common (numerator, first)
                * divide_common (rhs.denominator, second);
  int64_t denom = divide_common (denominator, second)
                  * divide_common (rhs.numerator, first);

  from_num_denom (num, denom);
  return *this;
}

void
rational::to_num_denom (int64_t &num, int64_t &denom) const
{
  num = numerator;
  denom = denominator;
}

void
rational::from_num_denom (int64_t num, int64_t denom)
{
  if (!denom)
    {
      numerator = num ? (num < 0 ? -1 : 1) : 0;
      denominator = 0;
      return;
    }

  if (!num)
    {
      numerator = 0;
      denominator = 1;
      return;
    }

  uint64_t divisor = gcd (magnitude (num), magnitude (denom));

  set_fraction (magnitude (num) / divisor, magnitude (denom) / divisor,
                (num < 0) != (denom < 0), numerator, denominator);
}
