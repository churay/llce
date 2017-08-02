## Part 14: Platform-Independent Game Memory ##

- In order to avoid errors with memory allocations, request all of the memory
  for the application up-front.
  - Partition the memory into chunks based on its use (e.g. component of the
    system that the memory is being used for).
  - Casey suggests partitioning the memory into two main chunks: a persistent
    chunk and a transient chunk.  The persistent chunk will be initialized with
    permanent variables (e.g. constants, managers) while the transient chunk
    will be filled fluidly based on application needs.
- Instead of using difficult-to-decipher flags such as `DEBUG`, try more
  descriptive flag names.
  - `SLOW`: Indicates that the application will take its time and do things
    like assert the correctness of the program state.
  - `INTERNAL`: Indicates that the application will be running on a well-known
    platform, which means the developer can take liberties in toying around
    with things like memory space and flex variable sizes (e.g. `int`).

## Part 21: Loading Game Code Dynamically ##

- In order to load code dynamically at the C/C++ level, split the application
  into multiple translation units and dynamically load the library files for
  the non-core libraries at run-time.
  - The core library will consist of all the main execution loop and
    platform-dependent functionality.
  - All of the other code (e.g. platform-independent game code) should be
    separated out into a collection of dynamically-loadable library files
    (e.g. shared object files on Unix).
- It's important to note that making the game's memory store managed by
  the application's core library will make it easier to perform dynamic
  code loads as there will never be a need to track down or reallocate
  memory blocks.
- When building a shared library in Unix, ensure that the compiler is
  given the `-fPIC` and `-shared` flags.
- 

## Part 22: Instantaneous Code Editing ##

- 

## Part 23: Looped-Live Code Editing ##

- 
