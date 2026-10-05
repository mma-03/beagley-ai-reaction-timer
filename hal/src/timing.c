// timing utilities implementation

#include "hal/timing.h"
#include <time.h>

static const long long NS_PER_MS = 1000 * 1000;
static const long long NS_PER_SECOND = 1000000000;
static const long long MS_PER_SECOND = 1000;

void sleepForMs(long long delayInMs) {
    long long delayNS = delayInMs * NS_PER_MS;
    int delaySec = delayNS / NS_PER_SECOND;
    int leftOver = delayNS % NS_PER_SECOND;

    struct timespec reqDelay = {delaySec, leftOver};
    nanosleep(&reqDelay, NULL);
}

long long getTimeInMs(void) {
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);

    long long sec = spec.tv_sec;
    long long nsec = spec.tv_nsec;

    return sec * MS_PER_SECOND + nsec / NS_PER_MS;
}