#ifndef LLCE_CONSTS_H
#define LLCE_CONSTS_H

#include <stdint.h>
#include <stdlib.h>
#include <iostream>

// TODO(JRC): Figure out some way to guarantee that the floating-point types
// will be the correct widths in a platform-independent way.
typedef uint32_t bool32_t;
typedef float float32_t;
typedef double float64_t;

// TODO(JRC): Define types that have same byte length as the pointer types
// for the current platform.

// TODO(JRC): Come up with better identifiers for the byte size operators below.
#define KILOBYTE_BL(v) ((v)*1024)
#define MEGABYTE_BL(v) (KILOBYTE_BL(v)*1024)
#define GIGABYTE_BL(v) (MEGABYTE_BL(v)*1024)

// NOTE(JRC): This is the maximum byte length according to the author of eCryptfs.
// (see: https://unix.stackexchange.com/a/32834)
#define MAXPATH_BL 4096

// TODO(JRC): Consider moving these macros to a different module.
// TODO(JRC): Consider changing these functions to use 'prinf' functionality
// instead of 'std::cerr' functionality.
// NOTE(JRC): Code inspired by Stack Overflow response:
// https://stackoverflow.com/a/3767883.
#ifdef DEBUG
#define LLCE_ASSERT_INFO(condition, message) \
    do { \
        if (! (condition)) { \
            std::cerr << "[INFO] (" << __FILE__ << ":" << __LINE__ << "): " << message << std::endl; \
        } \
    } while (false)
#else
#define LLCE_ASSERT_INFO(condition, message) do { } while(false)
#endif

#ifdef DEBUG
#define LLCE_ASSERT_DEBUG(condition, message) \
    do { \
        if (! (condition)) { \
            std::cerr << "[DEBUG] (" << __FILE__ << ":" << __LINE__ << "): " << message << std::endl; \
            exit(1); \
        } \
    } while (false)
#else
#define LLCE_ASSERT_DEBUG(condition, message) do { } while(false)
#endif

#define LLCE_ASSERT_ERROR(condition, message) \
    do { \
        if (! (condition)) { \
            std::cerr << "[ERROR] (" << __FILE__ << ":" << __LINE__ << "): " << message << std::endl; \
            exit(1); \
        } \
    } while (false)

#endif
