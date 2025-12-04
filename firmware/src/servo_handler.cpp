#include <Arduino.h>

#include "config.h"
#include "core/containers.h"

#include "servo_handler.h"
#include "alarm.h"

extern Servo servo_1;
extern Servo servo_left;
extern Servo servo_right;

ball_info balls[ball_count] = {
    {servo_left , kk::ball2, kk::ball2_slider_left, kk::ball2_slider_right},
    {servo_right, kk::ball3, kk::ball3_slider_left, kk::ball3_slider_right}
};

void ball_info::close() {
    if (!last_state) {
        servo_.write(get_right());
        last_state = true;
        db[id_] = true;
    }
}

void ball_info::release() {
    if (last_state) {
        servo_.write(get_left());
        last_state = false;
        db[id_] = false;
    }
}

LP_LISTENER_("update_servo", [](){

    for (int i = 0; i < ball_count; i++) {
        Serial.printf("Ball#%d: %d -- %d, %d\n", i, balls[i].get_left(), balls[i].get_right(), balls[i].get_state());
    }

    for (int i = 0; i < ball_count; i++) {
        bool state = balls[i].get_state();
        if (state != balls[i].last_state) {
            if (state) balls[i].close();
            else       balls[i].release();
        }
        balls[i].last_state = state;
    }
});
