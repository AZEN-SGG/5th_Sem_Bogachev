#include "elapsed_warning.h"

#include <chrono>
#include <climits>
#include <cstdio>
#include <cstring>
#include <thread>
#include <utility>

static void
wait_tick ()
{
  std::this_thread::sleep_for (std::chrono::milliseconds (20));
}

static int
test_lifetime ()
{
  auto before = std::chrono::steady_clock::now ();
  elapsed_warning original (1);
  auto after = std::chrono::steady_clock::now ();

  std::this_thread::sleep_for (std::chrono::milliseconds (1100));

  auto first = std::chrono::steady_clock::now ();
  int64_t lifetime = original.get_lifetime ();
  auto last = std::chrono::steady_clock::now ();
  auto lower = std::chrono::duration_cast<std::chrono::seconds> (
    first - after).count ();
  auto upper = std::chrono::duration_cast<std::chrono::seconds> (
    last - before).count ();

  if (lifetime < lower || lifetime > upper)
    {
      fprintf (stderr, "Invalid lifetime\n");
      return 1;
    }

  elapsed_warning moved (std::move (original));
  elapsed_warning target (INT_MAX);

  target = std::move (moved);
  const elapsed_warning &view = target;

  if (original.get_lifetime () || moved.get_lifetime ()
      || view.get_lifetime () < lifetime)
    {
      fprintf (stderr, "Move changed lifetime\n");
      return 1;
    }

  return 0;
}

int
main (int argc, char **argv)
{
  if (argc != 2)
    {
      fprintf (stderr, "Test mode is required\n");
      return 1;
    }

  if (!strcmp (argv[1], "quiet"))
    {
      elapsed_warning timer (INT_MAX);

      return timer.get_lifetime () < 0;
    }

  if (!strcmp (argv[1], "invalid"))
    {
      elapsed_warning timer (-1);

      return timer.get_lifetime () != 0;
    }

  if (!strcmp (argv[1], "default"))
    {
      elapsed_warning timer;

      wait_tick ();
      return timer.get_lifetime () < 0;
    }

  if (!strcmp (argv[1], "lifetime"))
    return test_lifetime ();

  if (!strcmp (argv[1], "assign"))
    {
      elapsed_warning first (0);
      elapsed_warning second (0);

      wait_tick ();
      if (&first.operator= (std::move (second)) != &first)
        return 1;

      wait_tick ();
      return second.get_lifetime () != 0;
    }

  if (!strcmp (argv[1], "self"))
    {
      elapsed_warning timer (0);
      elapsed_warning &alias = timer;

      wait_tick ();
      return &timer.operator= (std::move (alias)) != &timer;
    }

  if (!strcmp (argv[1], "inactive"))
    {
      elapsed_warning original (-1);
      elapsed_warning moved (std::move (original));
      elapsed_warning target (INT_MAX);

      target = std::move (moved);
      return original.get_lifetime () || moved.get_lifetime ()
             || target.get_lifetime ();
    }

  if (!strcmp (argv[1], "reuse"))
    {
      elapsed_warning original (0);
      elapsed_warning moved (std::move (original));

      original = elapsed_warning (INT_MAX);
      wait_tick ();
      return original.get_lifetime () < 0 || moved.get_lifetime () < 0;
    }

  fprintf (stderr, "Invalid test mode\n");
  return 1;
}
