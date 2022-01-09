#ifndef __GAME_OF_LIFE_TIME_CONTROL_H__
#define __GAME_OF_LIFE_TIME_CONTROL_H__

/// Used to perform an action at a target rate, such as setting
/// a target FPS.
struct TimeControl {

    float ticks_per_sec;
    unsigned long last_tick;

};

void sleepTillNextTick(struct TimeControl *self);
int hasNextTickPassed(struct TimeControl *self);
unsigned long timeBetweenTicks(struct TimeControl *self);
unsigned long timeNow();

#endif // __GAME_OF_LIFE_TIME_CONTROL_H__
