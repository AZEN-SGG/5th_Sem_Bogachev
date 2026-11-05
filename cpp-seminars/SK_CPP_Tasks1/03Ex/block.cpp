#include "block.h"

#include <cstdio>
#include <cstdint>
#include <cstring>
#include <new>

double *
block::allocate_block (size_t size)
{
  double *start = nullptr;

  if (size)
    {
      start = new (std::nothrow) double[size]();
      if (!start)
        {
          fprintf (stderr, "Cannot allocate block\n");
          return nullptr;
        }
    }

  free_block ();
  m_start = start;
  m_size = size;
  m_own = size != 0;
  return m_start;
}

void
block::link_block (double *start, size_t size)
{
  if (!start && size)
    {
      fprintf (stderr, "Invalid block pointer\n");
      return;
    }

  if (start == m_start)
    {
      m_size = size;
      return;
    }

  free_block ();
  m_start = start;
  m_size = size;
}

void
block::free_block ()
{
  if (m_own)
    delete[] m_start;

  m_start = nullptr;
  m_size = 0;
  m_own = false;
}

void
block::swap_block (block &rhs)
{
  double *start = m_start;
  size_t size = m_size;
  bool own = m_own;

  m_start = rhs.m_start;
  m_size = rhs.m_size;
  m_own = rhs.m_own;

  rhs.m_start = start;
  rhs.m_size = size;
  rhs.m_own = own;
}

void
block::copy_to (block &dest)
{
  size_t n = m_size < dest.m_size ? m_size : dest.m_size;

  if (n)
    memmove (dest.m_start, m_start, n * sizeof (double));
}

void
block::copy_from (block &src)
{
  src.copy_to (*this);
}

double
block::add_block (const block &rhs)
{
  size_t n = m_size < rhs.m_size ? m_size : rhs.m_size;

  for (size_t i = 0; i < n; ++i)
    m_start[i] += rhs.m_start[i];

  if (!m_size)
    return 0.;

  double result = m_start[0];

  for (size_t i = 1; i < m_size; ++i)
    if (m_start[i] > result)
      result = m_start[i];

  return result;
}

double
block::sub_block (const block &rhs)
{
  size_t n = m_size < rhs.m_size ? m_size : rhs.m_size;

  for (size_t i = 0; i < n; ++i)
    m_start[i] -= rhs.m_start[i];

  if (!m_size)
    return 0.;

  double result = m_start[0];

  for (size_t i = 1; i < m_size; ++i)
    if (m_start[i] < result)
      result = m_start[i];

  return result;
}

void
block::print_block ()
{
  size_t n = m_size < 20 ? m_size : 20;

  for (size_t i = 0; i < n; ++i)
    fprintf (stderr, "%e ", m_start[i]);
}
