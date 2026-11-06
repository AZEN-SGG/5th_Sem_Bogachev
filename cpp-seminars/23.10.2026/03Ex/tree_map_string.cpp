#include "tree_map_string.h"

tree_map_string::tree_map_string (const tree_map_string &val)
{
  root = clone (val.root);
  _size = val._size;
}

tree_map_string &
tree_map_string::operator= (const tree_map_string &val)
{
  if (this == &val)
    return *this;

  tree_map_string copy (val);

  *this = (tree_map_string &&)copy;
  return *this;
}

tree_map_string::tree_map_string (tree_map_string &&val)
{
  root = val.root;
  _size = val._size;
  val.root = nullptr;
  val._size = 0;
}

tree_map_string &
tree_map_string::operator= (tree_map_string &&val)
{
  if (this == &val)
    return *this;

  destroy (root);
  root = val.root;
  _size = val._size;
  val.root = nullptr;
  val._size = 0;
  return *this;
}

void
tree_map_string::destroy (tree_node *node)
{
  if (!node)
    return;

  destroy (node->left);
  destroy (node->right);
  delete node;
}

tree_map_string::tree_node *
tree_map_string::clone (const tree_node *node)
{
  if (!node)
    return nullptr;

  tree_node *copy = new tree_node{ node->key, node->value };

  try
    {
      copy->left = clone (node->left);
      copy->right = clone (node->right);
    }
  catch (...)
    {
      destroy (copy);
      throw;
    }
  return copy;
}

tree_map_string::tree_node *&
tree_map_string::find_link (const string &key)
{
  tree_node **link = &root;

  while (*link)
    {
      if ((*link)->key == key)
        break;

      if ((*link)->key < key)
        link = &(*link)->right;
      else
        link = &(*link)->left;
    }
  return *link;
}

const string &
tree_map_string::at (const string &it) const
{
  tree_node *node = root;

  while (node && !(node->key == it))
    {
      if (node->key < it)
        node = node->right;
      else
        node = node->left;
    }

  if (!node)
    throw "Key not found";
  return node->value;
}

string &
tree_map_string::at (const string &it)
{
  tree_node *node = find_link (it);

  if (!node)
    throw "Key not found";
  return node->value;
}

string &
tree_map_string::operator[] (const string &it)
{
  tree_node *&link = find_link (it);

  if (!link)
    {
      link = new tree_node{ it, string () };
      ++_size;
    }
  return link->value;
}

bool
tree_map_string::insert (const string &it, const string &what)
{
  tree_node *&link = find_link (it);

  if (link)
    return false;

  link = new tree_node{ it, what };
  ++_size;
  return true;
}

bool
tree_map_string::insert (string &&it, string &&what)
{
  if (&it == &what)
    return insert (it, what);

  tree_node *&link = find_link (it);

  if (link)
    return false;

  link = new tree_node{ (string &&)it, (string &&)what };
  ++_size;
  return true;
}

bool
tree_map_string::erase (const string &it)
{
  tree_node *&link = find_link (it);
  tree_node *node = link;

  if (!node)
    return false;

  if (!node->left)
    link = node->right;
  else if (!node->right)
    link = node->left;
  else
    {
      tree_node **next_link = &node->right;

      while ((*next_link)->left)
        next_link = &(*next_link)->left;

      tree_node *next = *next_link;

      *next_link = next->right;
      next->left = node->left;
      next->right = node->right;
      link = next;
    }

  delete node;
  --_size;
  return true;
}

size_t
tree_map_string::replace_values (tree_node *node, const string &what,
                                const string &val)
{
  if (!node)
    return 0;

  size_t count = node->value.replace (what, val);

  count += replace_values (node->left, what, val);
  count += replace_values (node->right, what, val);
  return count;
}

size_t
tree_map_string::replace (const string &what, const string &val)
{
  string needle (what);
  string replacement (val);

  return replace_values (root, needle, replacement);
}

size_t
tree_map_string::count_nodes (const tree_node *node)
{
  if (!node)
    return 0;

  return 1 + count_nodes (node->left) + count_nodes (node->right);
}

tree_map_string
tree_map_string::cut_subtree (const string &it)
{
  tree_map_string result;
  tree_node *&link = find_link (it);

  result.root = link;
  result._size = count_nodes (link);
  link = nullptr;
  _size -= result._size;
  return result;
}

size_t
tree_map_string::count_lt (const string &val) const
{
  size_t count = 0;
  tree_node *node = root;

  while (node)
    {
      if (node->key < val)
        {
          count += 1 + count_nodes (node->left);
          node = node->right;
        }
      else
        node = node->left;
    }
  return count;
}
