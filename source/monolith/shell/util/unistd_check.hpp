#ifndef UNISTD_CHECK_H
#define UNISTD_CHECK_H

#ifdef __has_include
#if __has_include(<unistd.h>)
#define UNISTD_AVAILABLE 1
#else
#define UNISTD_AVAILABLE 0
#endif
#else
#define UNISTD_AVAILABLE 0
#endif  // has_include

#endif  // UNISTD_CHECK_H