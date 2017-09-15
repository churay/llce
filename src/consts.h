#ifndef LLCE_CONSTS_H
#define LLCE_CONSTS_H

#include <stdint.h>

// TODO(JRC): Figure out some way to guarantee that the floating-point types
// will be the correct widths in a platform-independent way.
typedef uint32_t bool32_t;
typedef float float32_t;
typedef double float64_t;

// TODO(JRC): Come up with better identifiers for the byte size operators below.
#define kilobyte_bl(v) ((v)*1024)
#define megabyte_bl(v) (kilobyte_bl(v)*1024)
#define gigabyte_bl(v) (megabyte_bl(v)*1024)

// NOTE(JRC): This is the maximum byte length according to the author of eCryptfs.
// (see: https://unix.stackexchange.com/a/32834)
#define maxpath_bl 4096

#endif
