// timing utilities
// sleeping and reading clock

#ifndef _TIMING_H
#define _TIMING_H

// Sleep/block for the given number of milliseconds
void sleepForMs(long long delayInMs);

// Current time in milliseconds
long long getTimeInMs(void);

#endif