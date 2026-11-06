#ifndef HASH_MAP_STRING_H
#define HASH_MAP_STRING_H

#include "string.h"

class hash_map_string
{
public:
  hash_map_string () = default;
  ~hash_map_string () { clear (); }

  hash_map_string (const hash_map_string &val);
  hash_map_string & operator= (const hash_map_string &val);
  hash_map_string (hash_map_string &&val);
  hash_map_string & operator= (hash_map_string &&val);

  size_t size () const { return _size; }
  bool empty () const { return _size == 0; }
  const string & at (const string &it) const;
  string & at (const string &it);
  string & operator[] (const string &it);

  bool insert (const string &it, const string &what);
  bool insert (string &&it, string &&what);
  bool erase (const string &it);
  size_t replace (const string &what, const string &val);
  hash_map_string cut_subhash (unsigned char val);
  size_t count_lt (const string &val) const;

private:
  struct hash_node
  {
    string key;
    string value;
    hash_node *next = nullptr;
  };

  hash_node head[256];
  size_t _size = 0;

  hash_node *& find_link (const string &key);
  void clear ();
};

#endif
