// simple utility header for error handling
#ifndef DAY2_UTIL_H
#define DAY2_UTIL_H

#include <stdio.h>
#include <stdlib.h>

#define errif(cond, msg) do { if (cond) { perror(msg); exit(EXIT_FAILURE); } } while (0)

#endif // DAY2_UTIL_H
