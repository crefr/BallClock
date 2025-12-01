#pragma once
#include "servo_handler.h"

const int hall_treshold = 1024; // 4096 - no magnetic field ; 0 - max field
const int close_wait_time = 2500; // ms

struct hall_state {
    long time = 0;
    bool magnet = false;
    ball_info &ball;
    int pin = 0;

    hall_state(ball_info &ball, int pin): ball(ball), pin(pin) {}

    void tick();
    uint8_t getHue() {return (millis() - time)*0xFF / close_wait_time; }
};

extern hall_state hall1;
extern hall_state hall2;
