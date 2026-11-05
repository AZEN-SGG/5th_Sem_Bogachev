#ifndef UNIQUE_POINTER_ARRAY_H
#define UNIQUE_POINTER_ARRAY_H

class unique_pointer_array
{
public:
  unique_pointer_array () = default;
  explicit unique_pointer_array (int *pointer) { m_pointer = pointer; }
  ~unique_pointer_array () { delete[] m_pointer; }

  unique_pointer_array (const unique_pointer_array &) = delete;
  unique_pointer_array & operator= (const unique_pointer_array &) = delete;

  unique_pointer_array (unique_pointer_array &&rhs) noexcept
  {
    m_pointer = rhs.m_pointer;
    rhs.m_pointer = nullptr;
  }

  unique_pointer_array & operator= (unique_pointer_array &&rhs) noexcept
  {
    if (this == &rhs)
      return *this;

    delete[] m_pointer;
    m_pointer = rhs.m_pointer;
    rhs.m_pointer = nullptr;
    return *this;
  }

  int * get () { return m_pointer; }

private:
  int *m_pointer = nullptr;
};

#endif
