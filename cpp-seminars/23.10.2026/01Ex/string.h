#ifndef STRING_H
#define STRING_H

#include <cstddef>

class string
{
public:
  string () = default;
  string (char val, size_t count);
  string (const char *str);
  ~string () { delete[] data; }

  string (const string &val);
  string & operator= (const string &val);
  string (string &&val);
  string & operator= (string &&val);

  string operator+ (const string &val);
  string & operator+= (const string &val);
  bool operator== (const string &val);
  bool operator< (const string &val);
  char operator[] (size_t i) const { return c_str ()[i]; }

  bool contains (const string &substr) const;
  bool replace (const string &what, const string &val);
  const char * c_str () const { return data ? data : ""; }
  size_t size () const { return _size; }
  bool empty () const { return _size == 0; }

  string & append (const string &rhs);
  string & erase (size_t start, size_t end);
  string substr (size_t start, size_t end) const;
  size_t find (const string &substr) const;
  size_t rfind (const string &substr) const;

private:
  char *data = nullptr;
  size_t _size = 0;

  void allocate (size_t count);
  int compare (const string &rhs) const;
};

#endif
