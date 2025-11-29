#include <Arduino.h>

#include "config.h"
#include "core/containers.h"

#include "servo_handler.h"

static bool ball_last_state[ball_count] = {false, false, false};

extern Servo servo_1;
extern Servo servo_2;
extern Servo servo_3;

ball_info balls[ball_count] = {
    {servo_1, kk::ball1, kk::ball1_slider_left, kk::ball1_slider_right},
    {servo_2, kk::ball2, kk::ball2_slider_left, kk::ball2_slider_right},
    {servo_3, kk::ball3, kk::ball3_slider_left, kk::ball3_slider_right}
};


LP_LISTENER_("update_servo", [](){

    for (int i = 0; i < ball_count; i++) {
        Serial.printf("Ball#%d: %d -- %d, %d\n", i, balls[i].get_left(), balls[i].get_right(), balls[i].get_state());
    }

    for (int i = 0; i < ball_count; i++) {
        bool state = balls[i].get_state();
        if (state != ball_last_state[i]) {
            int angle = state ? balls[i].get_right() : balls[i].get_left();
            balls[i].servo_.write(angle);
        }
        ball_last_state[i] = state;
    }
});
