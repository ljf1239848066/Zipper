#ifdef HAVE_CONFIG_H
#	include "sysdefs.h"
#else
#	include <stddef.h>
#	include <inttypes.h>
#	include <limits.h>
#   include <stdbool.h>

#include <stdlib.h>
#include <assert.h>

#define lzma_attr_alloc_size(x)

#undef memzero
#define memzero(s, n) memset(s, 0, n)

// NOTE: Avoid using MIN() and MAX(), because even conditionally defining
// those macros can cause some portability trouble, since on some systems
// the system headers insist defining their own versions.
#define my_min(x, y) ((x) < (y) ? (x) : (y))
#define my_max(x, y) ((x) > (y) ? (x) : (y))

#ifndef ARRAY_SIZE
#	define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))
#endif
#endif
