#include "unique_pointer_object.h"
#include "unique_pointer_array.h"
#include "flexible_pointer.h"
#include "shared_pointer.h"
#include "copiable_unique_pointer.h"
#include "elapsed_warning.h"
#include "some_object.h"

#include <cstddef>
#include <cstdio>
#include <new>
#include <type_traits>
#include <utility>

static int checks = 0;
static int errors = 0;
static bool fail_allocation = false;

void *
operator new (size_t size, const std::nothrow_t &) noexcept
{
  if (fail_allocation)
    {
      fail_allocation = false;
      return nullptr;
    }

  return ::operator new (size);
}

static void
check (bool value, const char *name)
{
  ++checks;
  if (!value)
    {
      ++errors;
      fprintf (stderr, "Failed: %s\n", name);
    }
}

template <class pointer_type>
static void
check_unique_interface ()
{
  static_assert (std::is_default_constructible<pointer_type>::value,
                 "Default construction");
  static_assert (!std::is_copy_constructible<pointer_type>::value,
                 "No copy construction");
  static_assert (!std::is_copy_assignable<pointer_type>::value,
                 "No copy assignment");
  static_assert (std::is_nothrow_move_constructible<pointer_type>::value,
                 "Move construction");
  static_assert (std::is_nothrow_move_assignable<pointer_type>::value,
                 "Move assignment");
}

static void
reset_objects ()
{
  check (some_object::live == 0, "previous objects released");
  some_object::reset_counts ();
}

static void
check_released ()
{
  check (some_object::live == 0, "all objects released");
  check (some_object::created == some_object::destroyed,
         "each object deleted once");
  check (some_object::moved == 0, "pointer moves preserve objects");
  check (some_object::copy_assigned == 0 && some_object::move_assigned == 0,
         "pointer operations do not assign objects");
}

static void
test_unique_object ()
{
  reset_objects ();

  {
    unique_pointer_object empty;
    unique_pointer_object null (nullptr);
    some_object *pointer = new some_object (11);
    unique_pointer_object first (pointer);

    check (!empty.get () && !null.get (), "empty object pointers");
    check (first.get () == pointer, "adopt object pointer");

    unique_pointer_object second (std::move (first));

    check (!first.get () && second.get () == pointer, "move object pointer");
    first = std::move (second);
    check (!second.get () && first.get () == pointer, "reuse moved source");

    unique_pointer_object third (new some_object (22));

    check (&third.operator= (std::move (first)) == &third, "move returns self");
    check (some_object::destroyed == 1, "move releases old object");
    check (!first.get () && third.get () == pointer, "move into owner");

    unique_pointer_object &alias = third;

    third = std::move (alias);
    check (third.get () == pointer, "self move preserves object");
    third = std::move (empty);
    check (!third.get () && some_object::live == 0, "assign empty pointer");
    unique_pointer_object moved_empty (std::move (empty));
    check (!moved_empty.get (), "move empty object pointer");
  }

  check (some_object::copied == 0, "unique pointer does not copy object");
  check_released ();
}

static void
test_unique_array ()
{
  unique_pointer_array empty;
  unique_pointer_array null (nullptr);
  int *pointer = new int[4]{ 1, 2, 3, 4 };
  unique_pointer_array first (pointer);

  check (!empty.get () && !null.get (), "empty array pointers");
  check (first.get () == pointer, "adopt array pointer");

  unique_pointer_array second (std::move (first));

  check (!first.get () && second.get () == pointer, "move array pointer");
  check (second.get ()[3] == 4, "array contents survive move");

  unique_pointer_array third (new int[2]{ 5, 6 });

  check (&third.operator= (std::move (second)) == &third,
         "array move returns self");
  check (!second.get () && third.get () == pointer, "move into array owner");

  unique_pointer_array &alias = third;

  third = std::move (alias);
  check (third.get () == pointer, "array self move");
  first = std::move (third);
  check (!third.get () && first.get () == pointer, "reuse moved array source");
  first = std::move (empty);
  check (!first.get (), "release array through empty move");
}

static void
test_flexible_pointer ()
{
  reset_objects ();
  some_object *external = new some_object (42);

  {
    flexible_pointer empty;
    flexible_pointer borrowed (external, false);
    flexible_pointer moved (std::move (borrowed));

    check (!empty.get () && !empty.is_owner (), "empty flexible pointer");
    check (!borrowed.get () && !borrowed.is_owner (), "empty moved borrower");
    check (moved.get () == external && !moved.is_owner (),
           "move borrowed flag");

    flexible_pointer owner (new some_object (7), true);

    check (&owner.operator= (std::move (moved)) == &owner,
           "flexible move returns self");
    check (some_object::destroyed == 1, "replace owned flexible object");
    check (owner.get () == external && !owner.is_owner (),
           "adopt borrowed flag");

    flexible_pointer &alias = owner;

    owner = std::move (alias);
    check (owner.get () == external && !owner.is_owner (),
           "borrowed self move");
  }

  check (some_object::live == 1 && external->get_value () == 42,
         "borrowed object survives destruction");
  delete external;
  check_released ();
  reset_objects ();

  {
    some_object *pointer = new some_object (19);
    flexible_pointer first (pointer, true);
    flexible_pointer second (std::move (first));
    flexible_pointer empty;
    flexible_pointer third;

    check (second.get () == pointer && second.is_owner (), "move owned flag");
    check (!first.get () && !first.is_owner (), "empty moved owner");
    third = std::move (second);
    check (third.get () == pointer && third.is_owner (), "assign owned flag");

    flexible_pointer &alias = third;

    third = std::move (alias);
    check (third.get () == pointer && third.is_owner (), "owned self move");
    third = std::move (empty);
    check (!third.get () && !third.is_owner (), "clear flexible owner");

    const flexible_pointer null (nullptr, true);

    check (null.is_owner (), "preserve explicit null ownership flag");
  }

  check_released ();
  reset_objects ();
  external = new some_object (31);

  {
    flexible_pointer borrowed (external, false);
    flexible_pointer owner (new some_object (32), true);
    some_object *pointer = owner.get ();

    borrowed = std::move (owner);
    check (borrowed.get () == pointer && borrowed.is_owner (),
           "replace borrower with owner");
    check (external->get_value () == 31 && some_object::live == 2,
           "assignment preserves previous borrowed object");
  }

  check (some_object::live == 1 && external->get_value () == 31,
         "previous borrowed object survives owner destruction");
  delete external;
  check_released ();
}

static void
test_shared_pointer ()
{
  reset_objects ();

  {
    shared_pointer empty;
    shared_pointer null (nullptr);
    shared_pointer first (new some_object (11));
    some_object *pointer = first.get ();
    shared_pointer second (first);

    check (!empty.get () && !null.get (), "empty shared pointers");
    check (second.get () == pointer, "shared copy preserves address");
    check (some_object::created == 1, "shared copy does not create object");

    shared_pointer third (new some_object (22));

    check (&third.operator= (second) == &third, "shared copy returns self");
    check (some_object::destroyed == 1, "shared copy releases old group");
    first = second;
    second.get ()->set_value (77);
    check (first.get ()->get_value () == 77 && third.get ()->get_value () == 77,
           "shared copies observe changes");

    shared_pointer &alias = first;

    first = alias;
    first = std::move (alias);
    check (first.get () == pointer, "shared self assignments");

    shared_pointer moved (std::move (second));

    check (!second.get () && moved.get () == pointer,
           "shared move constructor");
    check (&third.operator= (std::move (moved)) == &third,
           "shared move returns self");
    check (!moved.get () && third.get () == pointer,
           "move within shared group");

    {
      shared_pointer copies[64];

      for (shared_pointer &copy : copies)
        {
          copy = first;
          check (copy.get () == pointer, "shared group member");
        }
    }

    check (some_object::live == 1, "group survives intermediate destruction");
    first = empty;
    check (!first.get () && some_object::live == 1, "release one shared owner");
    third = std::move (empty);
    check (!third.get () && some_object::live == 0,
           "release final shared owner");

    shared_pointer copied_empty (null);
    shared_pointer moved_empty (std::move (copied_empty));

    check (!copied_empty.get () && !moved_empty.get (), "copy and move empty");
  }

  check (some_object::copied == 0, "shared pointer does not copy object");
  check_released ();
}

static void
test_copiable_pointer ()
{
  reset_objects ();

  {
    copiable_unique_pointer empty;
    copiable_unique_pointer first (new some_object (11));
    copiable_unique_pointer second (first);

    check (second.get () != first.get (), "deep copy has different address");
    check (second.get ()->get_value () == 11, "deep copy preserves value");
    second.get ()->set_value (22);
    check (first.get ()->get_value () == 11, "deep copies are independent");

    copiable_unique_pointer third (new some_object (33));

    check (&third.operator= (second) == &third, "deep assignment returns self");
    check (third.get () != second.get () && third.get ()->get_value () == 22,
           "deep assignment copies object");
    check (some_object::destroyed == 1, "deep assignment releases old object");

    copiable_unique_pointer &alias = third;
    some_object *pointer = third.get ();
    int copies = some_object::copied;

    third = alias;
    third = std::move (alias);
    check (third.get () == pointer && some_object::copied == copies,
           "deep pointer self assignments");

    copiable_unique_pointer moved (std::move (third));

    check (!third.get () && moved.get () == pointer, "deep pointer move");
    check (some_object::copied == copies, "move does not clone object");
    check (&first.operator= (std::move (moved)) == &first,
           "deep move assignment returns self");
    check (!moved.get () && first.get () == pointer,
           "deep move adopts pointer");
    second = empty;
    check (!second.get (), "deep assignment from empty");
    copiable_unique_pointer copied_empty (empty);
    copiable_unique_pointer moved_empty (std::move (copied_empty));

    check (!copied_empty.get () && !moved_empty.get (), "deep empty copies");
    first = std::move (empty);
    check (some_object::live == 0, "deep owners released");
  }

  check_released ();
}

static void
test_allocation_errors ()
{
  reset_objects ();

  {
    some_object *pointer = new some_object (7);

    fail_allocation = true;
    shared_pointer failed (pointer);
    check (!failed.get (), "failed shared construction stays empty");
    check (some_object::live == 0, "failed shared construction deletes object");
    check (!fail_allocation, "shared construction attempted allocation");
  }

  check_released ();
  reset_objects ();

  {
    copiable_unique_pointer source (new some_object (8));

    fail_allocation = true;
    copiable_unique_pointer failed (source);
    check (!failed.get (), "failed deep construction stays empty");
    check (source.get ()->get_value () == 8 && some_object::live == 1,
           "failed deep construction preserves source");
    check (!fail_allocation, "deep construction attempted allocation");

    copiable_unique_pointer target (new some_object (9));
    some_object *previous = target.get ();
    int destroyed = some_object::destroyed;

    fail_allocation = true;
    check (&target.operator= (source) == &target,
           "failed assignment returns self");
    check (target.get () == previous && target.get ()->get_value () == 9,
           "failed assignment preserves target");
    check (some_object::destroyed == destroyed && some_object::live == 2,
           "failed assignment releases no existing objects");
    check (!fail_allocation, "deep assignment attempted allocation");
    target = source;
    check (target.get () != source.get () && target.get ()->get_value () == 8,
           "assignment succeeds after allocation failure");
  }

  check_released ();
}

int
main ()
{
  check_unique_interface<unique_pointer_object> ();
  check_unique_interface<unique_pointer_array> ();
  check_unique_interface<flexible_pointer> ();
  check_unique_interface<elapsed_warning> ();
  static_assert (std::is_copy_constructible<shared_pointer>::value,
                 "Shared copy construction");
  static_assert (std::is_copy_assignable<shared_pointer>::value,
                 "Shared copy assignment");
  static_assert (std::is_nothrow_move_constructible<shared_pointer>::value,
                 "Shared move construction");
  static_assert (std::is_nothrow_move_assignable<shared_pointer>::value,
                 "Shared move assignment");
  static_assert (std::is_copy_constructible<copiable_unique_pointer>::value,
                 "Deep copy construction");
  static_assert (std::is_copy_assignable<copiable_unique_pointer>::value,
                 "Deep copy assignment");

  test_unique_object ();
  test_unique_array ();
  test_flexible_pointer ();
  test_shared_pointer ();
  test_copiable_pointer ();
  test_allocation_errors ();
  printf ("Checks: %d, errors: %d\n", checks, errors);
  return errors ? 1 : 0;
}
