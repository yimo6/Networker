#include "../include/public.h"

long long getMicroTime(){
    struct timeval timer;
    gettimeofday(&timer, NULL);
    return (long long)timer.tv_sec * 1000 + timer.tv_usec / 1000;
}

