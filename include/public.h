#pragma once

#ifndef uint
typedef unsigned int uint;
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef bool
typedef enum {
    false = 0,
    true = 1
} bool;
#endif

#include <sys/time.h>

typedef struct {
    uint timeout; //超时毫秒
    long long start;
} AppTimer;

long long getMicroTime();
