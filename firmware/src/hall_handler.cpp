#include <Arduino.h>

#include "Looper.h"
#include "config.h"
#include "servo_handler.h"

const int hall_treshold = 1024; // 4096 - no magnetic field ; 0 - max field
const int close_wait_time = 2500; // ms

struct hall_state {
    long time = 0;
    bool magnet = false;
    ball_info &ball;
    int pin = 0;

    hall_state(ball_info &ball, int pin): ball(ball), pin(pin) {}

    void tick() {
        int signal = analogRead(pin);

        if (signal < hall_treshold) {
            long cur_time = millis();
            if (!magnet) {
                time = cur_time;
                magnet = true;
            } else if (cur_time - time > close_wait_time) {
                ball.close();
                time = cur_time;
            }
        } else {
            magnet = false;
        }
    }
};

extern ball_info balls[ball_count];

// static hall_state hall1(balls[0], HALL_1);
static hall_state hall2(balls[1], HALL_2);

LP_TICKER([](){
//    hall1.tick();
    hall2.tick();
});
