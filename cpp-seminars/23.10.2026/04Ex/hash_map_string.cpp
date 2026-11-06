#include "hash_map_string.h"

hash_map_string::hash_map_string (const hash_map_string &val)
{
  try
    {
      for (size_t i = 0; i < 256; ++i)
        {
          hash_node **link = &head[i].next;

          for (hash_node *node = val.head[i].next; node; node = node->next)
            {
              *link = new hash_node{ node->key, node->value };
              link = &(*link)->next;
            }
        }
    }
  catch (...)
    {
      clear ();
      throw;
    }
  _size = val._size;
}

hash_map_string &
hash_map_string::operator= (const hash_map_string &val)
{
  if (this == &val)
    return *this;

  hash_map_string copy (val);

  *this = (hash_map_string &&)copy;
  return *this;
}

hash_map_string::hash_map_string (hash_map_string &&val)
{
  for (size_t i = 0; i < 256; ++i)
    {
      head[i].next = val.head[i].next;
      val.head[i].next = nullptr;
    }

  _size = val._size;
  val._size = 0;
}

hash_map_string &
hash_map_string::operator= (hash_map_string &&val)
{
  if (this == &val)
    return *this;

  clear ();
  for (size_t i = 0; i < 256; ++i)
    {
      head[i].next = val.head[i].next;
      val.head[i].next = nullptr;
    }

  _size = val._size;
  val._size = 0;
  return *this;
}

void
hash_map_string::clear ()
{
  for (size_t i = 0; i < 256; ++i)
    while (head[i].next)
      {
        hash_node *node = head[i].next;

        head[i].next = node->next;
        delete node;
      }
  _size = 0;
}

hash_map_string::hash_node *&
hash_map_string::find_link (const string &key)
{
  unsigned char index = (unsigned char)key[0];
  hash_node **link = &head[index].next;

  while (*link && !((*link)->key == key))
    link = &(*link)->next;
  return *link;
}

const string &
hash_map_string::at (const string &it) const
{
  unsigned char index = (unsigned char)it[0];
  hash_node *node = head[index].next;

  while (node && !(node->key == it))
    node = node->next;

  if (!node)
    throw "Key not found";
  return node->value;
}

string &
hash_map_string::at (const string &it)
{
  hash_node *node = find_link (it);

  if (!node)
    throw "Key not found";
  return node->value;
}

string &
hash_map_string::operator[] (const string &it)
{
  hash_node *&link = find_link (it);

  if (!link)
    {
      link = new hash_node{ it, string () };
      ++_size;
    }
  return link->value;
}

bool
hash_map_string::insert (const string &it, const string &what)
{
  hash_node *&link = find_link (it);

  if (link)
    return false;

  link = new hash_node{ it, what };
  ++_size;
  return true;
}

bool
hash_map_string::insert (string &&it, string &&what)
{
  if (&it == &what)
    return insert (it, what);

  hash_node *&link = find_link (it);

  if (link)
    return false;

  link = new hash_node{ (string &&)it, (string &&)what };
  ++_size;
  return true;
}

bool
hash_map_string::erase (const string &it)
{
  hash_node *&link = find_link (it);
  hash_node *node = link;

  if (!node)
    return false;

  link = node->next;
  delete node;
  --_size;
  return true;
}

size_t
hash_map_string::replace (const string &what, const string &val)
{
  string needle (what);
  string replacement (val);
  size_t count = 0;

  for (size_t i = 0; i < 256; ++i)
    for (hash_node *node = head[i].next; node; node = node->next)
      count += node->value.replace (needle, replacement);
  return count;
}

hash_map_string
hash_map_string::cut_subhash (unsigned char val)
{
  hash_map_string result;

  result.head[val].next = head[val].next;
  head[val].next = nullptr;
  for (hash_node *node = result.head[val].next; node; node = node->next)
    ++result._size;

  _size -= result._size;
  return result;
}

size_t
hash_map_string::count_lt (const string &val) const
{
  size_t count = 0;

  for (size_t i = 0; i < 256; ++i)
    for (hash_node *node = head[i].next; node; node = node->next)
      if (node->key < val)
        ++count;
  return count;
}
