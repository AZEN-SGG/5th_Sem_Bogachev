#ifndef UNIQUE_POINTER_OBJECT_H
#define UNIQUE_POINTER_OBJECT_H

#include "some_object.h"

class unique_pointer_object
{
public:
  unique_pointer_object () = default;
  explicit unique_pointer_object (some_object *pointer) { m_pointer = pointer; }
  ~unique_pointer_object () { delete m_pointer; }

  unique_pointer_object (const unique_pointer_object &) = delete;
  unique_pointer_object & operator= (const unique_pointer_object &) = delete;

  unique_pointer_object (unique_pointer_object &&rhs) noexcept
  {
    m_pointer = rhs.m_pointer;
    rhs.m_pointer = nullptr;
  }

  unique_pointer_object & operator= (unique_pointer_object &&rhs) noexcept
  {
    if (this == &rhs)
      return *this;

    delete m_pointer;
    m_pointer = rhs.m_pointer;
    rhs.m_pointer = nullptr;
    return *this;
  }

  some_object * get () { return m_pointer; }

private:
  some_object *m_pointer = nullptr;
};

#endif
