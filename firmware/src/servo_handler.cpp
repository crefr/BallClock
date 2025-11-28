#include "settings.h"
#include <Arduino.h>

#include <GyverNTP.h>
#include <LittleFS.h>
#include <Looper.h>
#include <SettingsESP.h>
#include <WiFiConnector.h>
#include <Servo.h>

#include "config.h"
#include "core/containers.h"

extern Servo servo_1;
extern Servo servo_2;
extern Servo servo_3;

static bool ball_last_state[3] = {false, false, false};

struct ball_info {
private:
    size_t id_, left_id_, right_id_;
public:
    Servo &servo_;

    ball_info(Servo &servo, size_t id, size_t left_id, size_t right_id): id_(id), left_id_(left_id), right_id_(right_id), servo_(servo) {}

    bool get_state() { return db[id_].toBool(); }
    int  get_left()  { return db[left_id_].toInt(); }
    int  get_right() { return db[right_id_].toInt(); }
};


LP_TIMER(500, [](){
    ball_info balls[] = {
        {servo_1, kk::ball1, kk::ball1_slider_left, kk::ball1_slider_right},
        {servo_2, kk::ball2, kk::ball2_slider_left, kk::ball2_slider_right},
        {servo_3, kk::ball3, kk::ball3_slider_left, kk::ball3_slider_right}
    };

    for (int i = 0; i < 3; i++) {
        Serial.printf("Ball#%d: %d -- %d, %d\n", i, balls[i].get_left(), balls[i].get_right(), balls[i].get_state());
    }

    for (int i = 0; i < 3; i++) {
        bool state = balls[i].get_state();
        if (state != ball_last_state[i]) {
            int angle = state ? balls[i].get_right() : balls[i].get_left();
            balls[i].servo_.write(angle);
        }
        ball_last_state[i] = state;
    }
});
