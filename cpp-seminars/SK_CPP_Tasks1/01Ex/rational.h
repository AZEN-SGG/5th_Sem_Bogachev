#ifndef RATIONAL_H
#define RATIONAL_H

#include <cstdint>

class rational
{
public:
  rational ();
  rational (float value);
  rational (int64_t num, int64_t denom);

  float to_float () const;
  void from_float (float val);

  rational &add (const rational &rhs);
  rational &subtract (const rational &rhs);
  rational &multiply (const rational &rhs);
  rational &divide (const rational &rhs);

  void to_num_denom (int64_t &num, int64_t &denom) const;
  void from_num_denom (int64_t num, int64_t denom);

private:
  int64_t numerator{};
  int64_t denominator{};
};

#endif
