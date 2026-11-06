#include "string.h"

void
string::allocate (size_t count)
{
  if (count == (size_t)-1)
    throw "String is too long";

  char *buffer = nullptr;

  if (count)
    {
      buffer = new char[count + 1];
      buffer[count] = '\0';
    }

  delete[] data;
  data = buffer;
  _size = count;
}

string::string (char val, size_t count)
{
  allocate (count);
  for (size_t i = 0; i < count; ++i)
    data[i] = val;
}

string::string (const char *str)
{
  size_t count = 0;

  while (str[count])
    ++count;

  allocate (count);
  for (size_t i = 0; i < count; ++i)
    data[i] = str[i];
}

string::string (const string &val)
{
  allocate (val._size);
  for (size_t i = 0; i < _size; ++i)
    data[i] = val.data[i];
}

string &
string::operator= (const string &val)
{
  if (this == &val)
    return *this;

  string copy (val);

  *this = (string &&)copy;
  return *this;
}

string::string (string &&val)
{
  data = val.data;
  _size = val._size;
  val.data = nullptr;
  val._size = 0;
}

string &
string::operator= (string &&val)
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

string
string::operator+ (const string &val)
{
  if (val._size > (size_t)-1 - _size)
    throw "String is too long";

  string result;

  result.allocate (_size + val._size);
  for (size_t i = 0; i < _size; ++i)
    result.data[i] = data[i];
  for (size_t i = 0; i < val._size; ++i)
    result.data[_size + i] = val.data[i];

  return result;
}

string &
string::operator+= (const string &val)
{
  return append (val);
}

int
string::compare (const string &rhs) const
{
  size_t count = _size < rhs._size ? _size : rhs._size;

  for (size_t i = 0; i < count; ++i)
    {
      unsigned char left = (unsigned char)data[i];
      unsigned char right = (unsigned char)rhs.data[i];

      if (left < right)
        return -1;
      if (left > right)
        return 1;
    }

  if (_size < rhs._size)
    return -1;
  if (_size > rhs._size)
    return 1;
  return 0;
}

bool
string::operator== (const string &val)
{
  return compare (val) == 0;
}

bool
string::operator< (const string &val)
{
  return compare (val) < 0;
}

bool
string::contains (const string &substr) const
{
  return find (substr) != (size_t)-1;
}

bool
string::replace (const string &what, const string &val)
{
  size_t pos = find (what);

  if (pos == (size_t)-1 || what.compare (val) == 0)
    return false;

  size_t count = _size - what._size;

  if (val._size > (size_t)-1 - count)
    throw "String is too long";

  string result;

  result.allocate (count + val._size);
  for (size_t i = 0; i < pos; ++i)
    result.data[i] = data[i];
  for (size_t i = 0; i < val._size; ++i)
    result.data[pos + i] = val.data[i];
  for (size_t i = pos + what._size; i < _size; ++i)
    result.data[i - what._size + val._size] = data[i];

  *this = (string &&)result;
  return true;
}

string &
string::append (const string &rhs)
{
  if (!rhs.empty ())
    *this = *this + rhs;
  return *this;
}

string &
string::erase (size_t start, size_t end)
{
  if (start > end || end > _size)
    throw "Invalid string range";
  if (start == end)
    return *this;

  for (size_t i = end; i < _size; ++i)
    data[start + i - end] = data[i];

  _size -= end - start;
  data[_size] = '\0';
  return *this;
}

string
string::substr (size_t start, size_t end) const
{
  if (start > end || end > _size)
    throw "Invalid string range";

  string result;

  result.allocate (end - start);
  for (size_t i = start; i < end; ++i)
    result.data[i - start] = data[i];
  return result;
}

size_t
string::find (const string &substr) const
{
  if (substr._size > _size)
    return (size_t)-1;

  for (size_t i = 0; i <= _size - substr._size; ++i)
    {
      size_t j = 0;

      while (j < substr._size && data[i + j] == substr.data[j])
        ++j;
      if (j == substr._size)
        return i;
    }
  return (size_t)-1;
}

size_t
string::rfind (const string &substr) const
{
  if (substr._size > _size)
    return (size_t)-1;

  for (size_t i = _size - substr._size + 1; i > 0; --i)
    {
      size_t j = 0;

      while (j < substr._size && data[i - 1 + j] == substr.data[j])
        ++j;
      if (j == substr._size)
        return i - 1;
    }
  return (size_t)-1;
}
