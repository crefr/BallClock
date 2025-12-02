#pragma once

struct Alarm {
    bool played_today = true;
    bool active = false;
    bool ringing = false;
    int  opened_servos = 0;
    int  current_day;
    bool synced = false;

    int alarm_time = 1<<20;

    void set_state(bool on);

    void tick();

    void start();

    void stop();

    void set(int sec_from_midnight);
};

extern Alarm clock_alarm;
