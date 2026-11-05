#ifndef SHARED_POINTER_H
#define SHARED_POINTER_H

#include "some_object.h"

#include <cstddef>
#include <cstdio>
#include <new>

class shared_pointer
{
public:
  shared_pointer () = default;
  explicit shared_pointer (some_object *pointer)
  {
    if (!pointer)
      return;

    m_state = new (std::nothrow) shared_state{ pointer, 1 };
    if (!m_state)
      {
        fprintf (stderr, "Cannot allocate shared pointer state\n");
        delete pointer;
        return;
      }
  }

  ~shared_pointer () { free_pointer (); }

  shared_pointer (const shared_pointer &rhs)
  {
    m_state = rhs.m_state;
    if (m_state)
      ++m_state->owners;
  }

  shared_pointer & operator= (const shared_pointer &rhs)
  {
    if (this == &rhs)
      return *this;

    shared_state *state = rhs.m_state;

    if (state)
      ++state->owners;

    free_pointer ();
    m_state = state;
    return *this;
  }

  shared_pointer (shared_pointer &&rhs) noexcept
  {
    m_state = rhs.m_state;
    rhs.m_state = nullptr;
  }

  shared_pointer & operator= (shared_pointer &&rhs) noexcept
  {
    if (this == &rhs)
      return *this;

    free_pointer ();
    m_state = rhs.m_state;
    rhs.m_state = nullptr;
    return *this;
  }

  some_object * get () { return m_state ? m_state->pointer : nullptr; }

private:
  struct shared_state
  {
    some_object *pointer;
    size_t owners;
  };

  shared_state *m_state = nullptr;

  void free_pointer ()
  {
    if (!m_state)
      return;

    if (!--m_state->owners)
      {
        delete m_state->pointer;
        delete m_state;
      }

    m_state = nullptr;
  }
};

#endif
