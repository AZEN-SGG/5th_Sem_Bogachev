#ifndef FLEXIBLE_POINTER_H
#define FLEXIBLE_POINTER_H

#include "some_object.h"

class flexible_pointer
{
public:
  flexible_pointer () = default;
  flexible_pointer (some_object *pointer, bool owner)
  {
    m_pointer = pointer;
    m_owner = owner;
  }

  ~flexible_pointer ()
  {
    if (m_owner)
      delete m_pointer;
  }

  flexible_pointer (const flexible_pointer &) = delete;
  flexible_pointer & operator= (const flexible_pointer &) = delete;

  flexible_pointer (flexible_pointer &&rhs) noexcept
  {
    m_pointer = rhs.m_pointer;
    m_owner = rhs.m_owner;
    rhs.m_pointer = nullptr;
    rhs.m_owner = false;
  }

  flexible_pointer & operator= (flexible_pointer &&rhs) noexcept
  {
    if (this == &rhs)
      return *this;

    if (m_owner)
      delete m_pointer;

    m_pointer = rhs.m_pointer;
    m_owner = rhs.m_owner;
    rhs.m_pointer = nullptr;
    rhs.m_owner = false;
    return *this;
  }

  some_object * get () { return m_pointer; }
  bool is_owner () const { return m_owner; }

private:
  some_object *m_pointer = nullptr;
  bool m_owner = false;
};

#endif
