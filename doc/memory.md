## Linux Memory Management ##

### Memory Categories ###

When tracking the memory usage of a process using an application like `top`,
the usage statistics will be broken down into a number of subcategories.
These subcategories give insight regarding where the process' data has been
allocated and how the operating system is handling this allocation.  The
following list describes the meaning of each subcategory in detail:

- **Virtual Memory**: The space of memory that has been allocated by the process
  but not necessarily mapped to physical memory by the operating system.  This
  category generally applies to memory that has been allocated (e.g. using `malloc`
  or `mmap` in C) but not yet read/written.
- **Resident Memory**: The space of memory that has been mapped to physical
  memory by the operating system.  This set of memory can be thought of as the
  "live" memory for the process.
- **Shared Memory**: The space of memory that spans the subset of the virtual
  memory that can be shared with other processes.  Generally, the memory that
  falls into this category is memory used by loading shared libraries (primarily
  their binary symbols).

### Memory Workflow ###

Within an application, memory allocations will cause the process' **virtual memory**
space to expand.  When this memory is accessed or written to by the process,
this memory will become **resident memory** for the machine as well.  The amount
of **resident memory** for any given process will fluctuate based on the process
environment for the machine and may shrink if many rival applications begin to
consume memory concurrently.

While the unknown environment of the host machine makes it hard to optimize a
process' memory usage in all cases, there are a number of strategies that can
be followed to improve memory performance:

- Allocate most (if not all) of the application's memory on initialization.
  This ensure that the application's memory blocks are subsequent, which
  greatly helps with cache performance.
- Allocate memory for structures that are commonly used together in adjacent
  or near-adjacent memory blocks, which will also help with cache performance.

### Resources ###

The information for this listing was pulled from the following resources:

- [Server Fault: What Does Virtual Memory Size Mean?](https://serverfault.com/a/138625)
- [MG's Notes: VIRT, RES, and SHR Memory](http://mugurel.sumanariu.ro/linux/the-difference-among-virt-res-and-shr-in-top-output/)
- [Stack Overflow: What are RSS and VSZ in Linux Memory Management?](https://stackoverflow.com/a/21049737/837221)
