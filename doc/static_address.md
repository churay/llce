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

Since a static address must remain consistent between executions of an application
to be useful, it needs to be determined and set at compile time for any target
process binary. The following procedure can be leveraged to determine current size
of an application binary and use this information to set an appropriate process
base address:

1. `getconf PAGE_SIZE`: The output of this command is the page size for the current
   machine, which is 4096 (i.e. 2^12) for many Unix-based systems. This value will
   be used to inform the offset of the final static address (which will be aligned
   to the page size to minimize page occupancy and (consequently) maximize performance).
1. `ps -aux | grep [process name]`: This command will print out process metadata
   for processes containing `[process name]` in their executable name. The piece
   of metadata required for the next step is the process ID, which is given in the
   second column of this command's output. The code listing below contains example
   output for this command with the process ID highlighted:
   ```
   user **pid** cpu% mem% vmem rmem tty stat start time command
   ```
1. `sudo pmap -x [process id]`: Feeding the process ID extracted during the last
   step into the `pmap` command outputs all the memory address information for each
   memory chunk and instruction set loaded by the application. The address space
   for an application will be split into two main components: the application chunk
   (lower address values) and the library chunk (higher address values). The static
   address value should be chosen to be a location between these two chunks that
   accomodates the projected development needs of the application. If the application
   is projected to be library-heavy, a static address closer to the lower end should
   be selected, and vice versa. Additionally, the chosen static address should be
   aligned to the page size determined in the earlier steps of this process.

Ultimately, these steps could be automated to make updating loop-live applications
even easier, but the time and effort it would take to develop the associated scripts
isn't worthwhile for most application (which may end up changing the static address
for an application once, if at all).

### Resources ###

The information for this listing was pulled from the following resources:

- [Process Address Space](https://www.kernel.org/doc/gorman/html/understand/understand007.html)
- [Stack Overflow: Measure a Process' Memory Usage](https://stackoverflow.com/a/2816070/837221)
