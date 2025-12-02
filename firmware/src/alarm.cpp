
#include "piezo_player.h"
#include "servo_handler.h"

#include "alarm.h"

#include <GyverNTP.h>
#include <Looper.h>
#include <GyverDBFile.h>

#include "servo_handler.h"
extern ball_info balls[ball_count];

void Alarm::set_state(bool on) {
    active = on;
    if (!on && ringing) {
        pieza.stop();
        ringing = false;
    }
    synced = false;
    Serial.printf("Alarm state: %d, played today = %d\n", on, played_today);
}

void Alarm::tick() {
    if (!active) return;

    if (!NTP.synced()) {
        return;
    }

    Datime dt = NTP;

    if (!synced) {
        current_day = dt.day;
        played_today = dt.daySeconds() > alarm_time;
        synced = true;
    }

    if (dt.day != current_day) {
        current_day = dt.day;
        played_today = false;
    }

    if (ringing && (balls[1].last_state + balls[2].last_state) == 2) {
        stop();
        Serial.printf("Stopping alarm\n");
        return;
    }

    if (!played_today && !ringing && (dt.daySeconds() >= alarm_time) ) {
        Serial.printf("Starting alarm, t = %d, alarm = %d\n", dt.daySeconds(), alarm_time);
        start();
    }
}

void Alarm::start() {
    ringing = true;
    pieza.play();

    db[kk::ball2] = false;
    db[kk::ball3] = false;

    Looper.pushEvent("update_servo");
}

void Alarm::stop() {
    pieza.stop();
    played_today = true;
    ringing = false;
}

void Alarm::set(int sec_from_midnight) {
    // Datime dt = NTP;
    alarm_time = sec_from_midnight;
    synced = false;
    // played_today = dt.daySeconds() > sec_from_midnight;
    Serial.printf("Set alarm to %d; played_today = %d\n",
                  alarm_time, played_today);
}


Alarm clock_alarm;


