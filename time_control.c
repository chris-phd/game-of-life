#include "time_control.h"

#include <stdio.h>
#include <time.h>
#include <math.h>

void sleepTillNextTick(struct TimeControl *self) {
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
    // TODO, reduce repetition with sleepTillNextTick
    unsigned long now = timeNow();
    unsigned long total = timeBetweenTicks(self);
    unsigned long elapsed = now - self->last_tick;
    return elapsed > total;
}

unsigned long timeBetweenTicks(struct TimeControl *self) {
    double us_per_sec = 1000000.0;
    return (unsigned long)((1.0 / self->ticks_per_sec ) * us_per_sec);
}

unsigned long timeNow() {
    const unsigned long us_per_sec = 1000000;
    const double us_per_ns  = 0.001;
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    return (ts.tv_sec * us_per_sec) + (unsigned long) (ts.tv_nsec * us_per_ns);
}
