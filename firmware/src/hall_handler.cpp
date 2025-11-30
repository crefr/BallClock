#include <Arduino.h>

#include "Looper.h"
#include "config.h"
#include "servo_handler.h"

const int hall_treshold = 1024; // 4096 - no magnetic field ; 0 - max field
const int close_wait_time = 1500; // ms

struct hall_state {
    long time = 0;
    bool magnet = false;
    ball_info &ball;

    hall_state(ball_info &ball): ball(ball) {}

    void tick() {
        int signal = analogRead(HALL_3);

        if (signal < hall_treshold) {
            long cur_time = millis();
            if (!magnet) {
                time = cur_time;
                magnet = true;
            } else if (cur_time - time > close_wait_time) {
                ball.close();
            }
        } else {
            magnet = false;
        }
    }
};

extern ball_info balls[ball_count];

static hall_state hall3(balls[2]);

LP_TICKER([](){
   hall3.tick();
});
