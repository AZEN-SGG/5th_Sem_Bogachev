#ifndef TREE_MAP_STRING_H
#define TREE_MAP_STRING_H

#include "string.h"

class tree_map_string
{
public:
  tree_map_string () = default;
  ~tree_map_string () { destroy (root); }

  tree_map_string (const tree_map_string &val);
  tree_map_string & operator= (const tree_map_string &val);
  tree_map_string (tree_map_string &&val);
  tree_map_string & operator= (tree_map_string &&val);

  size_t size () const { return _size; }
  bool empty () const { return _size == 0; }
  const string & at (const string &it) const;
  string & at (const string &it);
  string & operator[] (const string &it);

  bool insert (const string &it, const string &what);
  bool insert (string &&it, string &&what);
  bool erase (const string &it);
  size_t replace (const string &what, const string &val);
  tree_map_string cut_subtree (const string &it);
  size_t count_lt (const string &val) const;

private:
  struct tree_node
  {
    string key;
    string value;
    tree_node *left = nullptr;
    tree_node *right = nullptr;
  };

  tree_node *root = nullptr;
  size_t _size = 0;

  tree_node *& find_link (const string &key);
  static void destroy (tree_node *node);
  static tree_node * clone (const tree_node *node);
  static size_t count_nodes (const tree_node *node);
  static size_t replace_values (tree_node *node, const string &what,
                                const string &val);
};

#endif
