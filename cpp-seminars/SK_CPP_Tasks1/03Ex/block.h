#ifndef BLOCK_H
#define BLOCK_H

#include <cstddef>

class block
{
public:
  block () = default;
  block (size_t size) { allocate_block (size); }
  block (double *start, size_t size) { link_block (start, size); }
  ~block () { free_block (); }

  block (const block &) = delete;
  block &operator= (const block &) = delete;

  double *allocate_block (size_t size);
  void link_block (double *start, size_t size);
  void free_block ();

  void swap_block (block &rhs);
  void copy_to (block &dest);
  void copy_from (block &src);

  double add_block (const block &rhs);
  double sub_block (const block &rhs);
  void print_block ();

private:
  double *m_start{};
  size_t m_size{};
  bool m_own = false;
};

#endif
