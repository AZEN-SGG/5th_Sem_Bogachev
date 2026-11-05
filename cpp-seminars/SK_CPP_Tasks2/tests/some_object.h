#ifndef SOME_OBJECT_H
#define SOME_OBJECT_H

class some_object
{
public:
  some_object ()
  {
    ++created;
    ++live;
  }

  explicit some_object (int value)
  {
    m_value = value;
    ++created;
    ++live;
  }

  ~some_object ()
  {
    ++destroyed;
    --live;
  }

  some_object (const some_object &rhs)
  {
    m_value = rhs.m_value;
    ++created;
    ++copied;
    ++live;
  }

  some_object & operator= (const some_object &rhs)
  {
    m_value = rhs.m_value;
    ++copy_assigned;
    return *this;
  }

  some_object (some_object &&rhs) noexcept
  {
    m_value = rhs.m_value;
    rhs.m_value = 0;
    ++created;
    ++moved;
    ++live;
  }

  some_object & operator= (some_object &&rhs) noexcept
  {
    if (this == &rhs)
      return *this;

    m_value = rhs.m_value;
    rhs.m_value = 0;
    ++move_assigned;
    return *this;
  }

  int get_value () const { return m_value; }
  void set_value (int value) { m_value = value; }

  static void reset_counts ()
  {
    created = 0;
    destroyed = 0;
    copied = 0;
    moved = 0;
    copy_assigned = 0;
    move_assigned = 0;
    live = 0;
  }

  static int created;
  static int destroyed;
  static int copied;
  static int moved;
  static int copy_assigned;
  static int move_assigned;
  static int live;

private:
  int m_value = 0;
};

#endif
