#include "vector_string.h"

static size_t
combined_size (size_t left, size_t right)
{
  if (right > (size_t)-1 - left)
    throw "Vector is too long";
  return left + right;
}

void
vector_string::allocate (size_t count)
{
  string *buffer = nullptr;

  if (count)
    buffer = new string[count];

  delete[] data;
  data = buffer;
  _size = count;
}

vector_string::vector_string (const string &val, size_t count)
{
  vector_string result;

  result.allocate (count);
  for (size_t i = 0; i < count; ++i)
    result.data[i] = val;

  *this = (vector_string &&)result;
}

vector_string::vector_string (const vector_string &val)
{
  vector_string result;

  result.allocate (val._size);
  for (size_t i = 0; i < val._size; ++i)
    result.data[i] = val.data[i];

  *this = (vector_string &&)result;
}

vector_string &
vector_string::operator= (const vector_string &val)
{
  if (this == &val)
    return *this;

  vector_string copy (val);

  *this = (vector_string &&)copy;
  return *this;
}

vector_string::vector_string (vector_string &&val)
{
  data = val.data;
  _size = val._size;
  val.data = nullptr;
  val._size = 0;
}

vector_string &
vector_string::operator= (vector_string &&val)
{
  if (this == &val)
    return *this;

  delete[] data;
  data = val.data;
  _size = val._size;
  val.data = nullptr;
  val._size = 0;
  return *this;
}

vector_string
vector_string::operator+ (const vector_string &val)
{
  vector_string result;

  result.allocate (combined_size (_size, val._size));
  for (size_t i = 0; i < _size; ++i)
    result.data[i] = data[i];
  for (size_t i = 0; i < val._size; ++i)
    result.data[_size + i] = val.data[i];
  return result;
}

vector_string &
vector_string::operator+= (const vector_string &val)
{
  return append (val);
}

vector_string &
vector_string::push_back (string &&str)
{
  return push (str, _size);
}

vector_string &
vector_string::push_front (string &&str)
{
  return push (str, 0);
}

vector_string &
vector_string::push (const string &str, size_t pos)
{
  vector_string result;

  result.allocate (combined_size (_size, 1));
  result.data[pos] = str;
  for (size_t i = 0; i < pos; ++i)
    result.data[i] = (string &&)data[i];
  for (size_t i = pos; i < _size; ++i)
    result.data[i + 1] = (string &&)data[i];

  *this = (vector_string &&)result;
  return *this;
}

vector_string &
vector_string::append (vector_string &&rhs)
{
  if (this == &rhs)
    return append ((const vector_string &)rhs);
  if (rhs.empty ())
    return *this;

  vector_string result;

  result.allocate (combined_size (_size, rhs._size));
  for (size_t i = 0; i < _size; ++i)
    result.data[i] = (string &&)data[i];
  for (size_t i = 0; i < rhs._size; ++i)
    result.data[_size + i] = (string &&)rhs.data[i];

  delete[] rhs.data;
  rhs.data = nullptr;
  rhs._size = 0;
  *this = (vector_string &&)result;
  return *this;
}

vector_string &
vector_string::append (const vector_string &rhs)
{
  if (!rhs.empty ())
    *this = *this + rhs;
  return *this;
}

vector_string &
vector_string::erase (size_t start, size_t end)
{
  if (start > end || end > _size)
    throw "Invalid vector range";
  if (start == end)
    return *this;

  vector_string result;

  result.allocate (_size - (end - start));
  for (size_t i = 0; i < start; ++i)
    result.data[i] = (string &&)data[i];
  for (size_t i = end; i < _size; ++i)
    result.data[start + i - end] = (string &&)data[i];

  *this = (vector_string &&)result;
  return *this;
}

size_t
vector_string::find (const string &str) const
{
  for (size_t i = 0; i < _size; ++i)
    if (data[i] == str)
      return i;
  return _size;
}

size_t
vector_string::rfind (const string &str) const
{
  for (size_t i = _size; i > 0; --i)
    if (data[i - 1] == str)
      return i - 1;
  return _size;
}

vector_string &
vector_string::replace (const string &what, const string &val)
{
  string needle (what);
  string replacement (val);

  for (size_t i = 0; i < _size; ++i)
    data[i].replace (needle, replacement);
  return *this;
}

vector_string
vector_string::subvector (size_t start, size_t end) const
{
  if (start > end || end > _size)
    throw "Invalid vector range";

  vector_string result;

  result.allocate (end - start);
  for (size_t i = start; i < end; ++i)
    result.data[i - start] = data[i];
  return result;
}
