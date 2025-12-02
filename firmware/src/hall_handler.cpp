#include <Arduino.h>

#include "Looper.h"
#include "config.h"
#include "servo_handler.h"

#include "hall_handler.h"

void hall_state::tick() {
    int signal = analogRead(pin);

    if (signal < hall_treshold) {
        long cur_time = millis();
        if (!magnet) {
            time = cur_time;
            magnet = true;
        } else if (cur_time - time > close_wait_time) {
            ball.close();
            time = LONG_MAX/2;
        }
    } else {
        magnet = false;
    }
}

extern ball_info balls[ball_count];

hall_state hall1(balls[2], HALL_1);
hall_state hall2(balls[1], HALL_2);

LP_TICKER([](){
    hall1.tick();
    hall2.tick();
});
