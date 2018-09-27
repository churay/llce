## Procedure for Finding a Static Address for Process "Dynamic" Memory ##

### Motivation: Why Use a Static Address? ###

When writing an application that supports in-memory restarts and dynamic
code hot-swapping (two key components of any application that aspires to
support loop-live code editing), it's critical to architect the application
so that stateful process memory can easily be loaded and flushed at any
given time. For an arbitrarily constructed process, fulfilling this requirement
is nigh insurmountable as it requires tracking down each piece of stateful
memory and normalizing contained memory pointers so that they're address-space-
independent. In order to circumvent these issues and to make enabling loop-live
code editing as easy as possible, the Handmade Hero code leverages two
foundational design decisions:

- **Pre-Allocation**: A memory allocation strategy that involves allocating
  all of a process' stateful memory at the beginning of the process. Not only
  does this strategy make it trivial to find a process' stateful memory, but
  it also improves cache performance by ensuring all of a process' memory is
  packed into a contiguous and sequential segment of memory.
- **Static Addressing**: A memory block allocation method that allows an
  application to reserve a segment of memory at a static virtual base address.
  Utilizing this technique within an application allows pointers to data within
  the statically allocated address space to remain unchanged between different
  runs of the application.

With all of that being said, it's clear that static addressing plays an important
role in keeping the implementation for a loop-live code editing application simple
and streamlined.

### Process: Finding an Adequate Static Address ###

Since a static address must remain constant between executions of an application
for it to be useful, it needs to be determined and set before the application even
initializes.

1. (TODO: Add Steps)
1. (TODO: Add Steps)


### Resources ###

The information for this listing was pulled from the following resources:

- [Process Address Space](https://www.kernel.org/doc/gorman/html/understand/understand007.html)
- [Stack Overflow: Measure a Process' Memory Usage](https://stackoverflow.com/a/2816070/837221)
