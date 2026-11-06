#ifndef VECTOR_STRING_H
#define VECTOR_STRING_H

#include "string.h"

class vector_string
{
public:
  vector_string () = default;
  vector_string (const string &val, size_t count);
  ~vector_string () { delete[] data; }

  vector_string (const vector_string &val);
  vector_string & operator= (const vector_string &val);
  vector_string (vector_string &&val);
  vector_string & operator= (vector_string &&val);

  vector_string operator+ (const vector_string &val);
  vector_string & operator+= (const vector_string &val);
  const string & operator[] (size_t i) const { return data[i]; }
  string & operator[] (size_t i) { return data[i]; }
  size_t size () const { return _size; }
  bool empty () const { return _size == 0; }

  vector_string & push_back (string &&str);
  vector_string & push_front (string &&str);
  vector_string & append (vector_string &&rhs);
  vector_string & append (const vector_string &rhs);
  vector_string & erase (size_t start, size_t end);
  size_t find (const string &str) const;
  size_t rfind (const string &str) const;
  vector_string & replace (const string &what, const string &val);
  vector_string subvector (size_t start, size_t end) const;

private:
  string *data = nullptr;
  size_t _size = 0;

  void allocate (size_t count);
  vector_string & push (const string &str, size_t pos);
};

#endif
