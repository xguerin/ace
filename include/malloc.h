#ifndef ACE_MALLOC_H_
#define ACE_MALLOC_H_

#if defined(__OpenBSD__)
#include <stdlib.h>
#elif defined(__linux__)
#include "/usr/include/malloc.h"
#else
#error "Unsupported architecture"
#endif

#endif
