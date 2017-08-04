## Linux Memory Management ##

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

The information for this listing was pulled from the following resources:

- [Server Fault: What Does Virtual Memory Size Mean?](https://serverfault.com/a/138625)
- [MG's Notes: VIRT, RES, and SHR Memory](http://mugurel.sumanariu.ro/linux/the-difference-among-virt-res-and-shr-in-top-output/)
