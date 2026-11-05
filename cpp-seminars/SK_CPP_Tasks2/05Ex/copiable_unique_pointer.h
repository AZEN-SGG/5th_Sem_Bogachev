#ifndef COPIABLE_UNIQUE_POINTER_H
#define COPIABLE_UNIQUE_POINTER_H

#include "some_object.h"

#include <cstdio>
#include <new>

class copiable_unique_pointer
{
public:
  copiable_unique_pointer () = default;
  explicit copiable_unique_pointer (some_object *pointer)
  {
    m_pointer = pointer;
  }

  ~copiable_unique_pointer () { delete m_pointer; }

  copiable_unique_pointer (const copiable_unique_pointer &rhs)
  {
    if (!rhs.m_pointer)
      return;

    m_pointer = new (std::nothrow) some_object (*rhs.m_pointer);
    if (!m_pointer)
      {
        fprintf (stderr, "Cannot allocate object copy\n");
        return;
      }
  }

  copiable_unique_pointer & operator= (const copiable_unique_pointer &rhs)
  {
    if (this == &rhs)
      return *this;

    some_object *pointer = nullptr;

    if (rhs.m_pointer)
      {
        pointer = new (std::nothrow) some_object (*rhs.m_pointer);
        if (!pointer)
          {
            fprintf (stderr, "Cannot allocate object copy\n");
            return *this;
          }
      }

    delete m_pointer;
    m_pointer = pointer;
    return *this;
  }

  copiable_unique_pointer (copiable_unique_pointer &&rhs) noexcept
  {
    m_pointer = rhs.m_pointer;
    rhs.m_pointer = nullptr;
  }

  copiable_unique_pointer & operator= (copiable_unique_pointer &&rhs) noexcept
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
