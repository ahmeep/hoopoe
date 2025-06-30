#include "time.h"

#include <time.h>

uint64_t hoopoe_time_milliseconds()
{
    struct timespec time;
    clock_gettime(CLOCK_REALTIME, &time);
    return time.tv_sec * 1000 + time.tv_nsec / 1000000;
}
