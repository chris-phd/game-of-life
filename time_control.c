#include "time_control.h"

#include <stdio.h>
#include <time.h>
#include <math.h>

void sleepTillNextTick(struct TimeControl *self) {
    fprintf(stderr, "time_control::sleepTillNextTick: \n");

    unsigned long now = timeNow();
    unsigned long total = timeBetweenTicks(self);
    unsigned long elapsed = now - self->last_tick;
    if (elapsed > total) {
        self->last_tick = now;
        return;
    }
    usleep(total - elapsed);
    self->last_tick = timeNow();
    return;
}

int hasNextTickPassed(struct TimeControl *self) {
    fprintf(stderr, "time_control::hasNextTickPassed: \n");
    return 1;
}

unsigned long timeBetweenTicks(struct TimeControl *self) {
    fprintf(stderr, "time_control::timeBetweenTicks: \n");
    double us_per_sec = 1000000.0;
    return (unsigned long)((1.0 / self->ticks_per_sec ) * us_per_sec);
}

unsigned long timeNow() {
    fprintf(stderr, "time_control::timeNow: \n");
    
    const unsigned long us_per_sec = 1000000;
    const double us_per_ns  = 0.001;
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    return (ts.tv_sec * us_per_sec) + (unsigned long) (ts.tv_nsec * us_per_ns);
}
