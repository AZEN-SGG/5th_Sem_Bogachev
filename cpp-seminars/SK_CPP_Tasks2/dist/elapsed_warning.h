#ifndef ELAPSED_WARNING_H
#define ELAPSED_WARNING_H

#include <cstdint>
#include <cstdio>
#include <ctime>

class elapsed_warning
{
public:
  elapsed_warning () : elapsed_warning (0) {}

  explicit elapsed_warning (int seconds)
  {
    if (seconds < 0)
      {
        fprintf (stderr, "Invalid warning interval\n");
        return;
      }

    m_start = get_time ();
    if (m_start < 0)
      return;

    m_limit = seconds;
    m_active = true;
  }

  ~elapsed_warning () { free_warning (); }

  elapsed_warning (const elapsed_warning &) = delete;
  elapsed_warning & operator= (const elapsed_warning &) = delete;

  elapsed_warning (elapsed_warning &&rhs) noexcept
  {
    m_start = rhs.m_start;
    m_limit = rhs.m_limit;
    m_active = rhs.m_active;
    rhs.m_active = false;
  }

  elapsed_warning & operator= (elapsed_warning &&rhs) noexcept
  {
    if (this == &rhs)
      return *this;

    free_warning ();
    m_start = rhs.m_start;
    m_limit = rhs.m_limit;
    m_active = rhs.m_active;
    rhs.m_active = false;
    return *this;
  }

  int64_t get_lifetime () const
  {
    if (!m_active)
      return 0;

    int64_t now = get_time ();

    if (now < 0)
      return 0;

    return (now - m_start) / ns_per_sec;
  }

private:
  static constexpr int64_t ns_per_sec = 1000000000;

  int64_t m_start = 0;
  int m_limit = 0;
  bool m_active = false;

  static int64_t get_time ()
  {
    timespec time{};

    if (clock_gettime (CLOCK_MONOTONIC, &time))
      {
        fprintf (stderr, "Cannot read monotonic time\n");
        return -1;
      }

    return (int64_t)time.tv_sec * ns_per_sec + time.tv_nsec;
  }

  void free_warning ()
  {
    if (!m_active)
      return;

    m_active = false;
    int64_t now = get_time ();

    if (now < 0)
      return;

    if (now - m_start > (int64_t)m_limit * ns_per_sec)
      fprintf (stderr, "1");
  }
};

#endif
