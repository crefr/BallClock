#pragma once

#include <LittleFS.h>
#include <Looper.h>
#include <Servo.h>

#include "settings.h"


struct ball_info {
private:
    size_t id_, left_id_, right_id_;
public:
    Servo &servo_;
    bool last_state = false;

    ball_info(Servo &servo, size_t id, size_t left_id, size_t right_id): id_(id), left_id_(left_id), right_id_(right_id), servo_(servo) {}

    size_t get_id() const { return id_; }

    void set_limits(float left, float right) {
        db[left_id_] = left;
        db[right_id_] = right;
    }

    bool get_state() const { return db[id_].toBool(); }
    int  get_left()  const { return db[left_id_].toInt(); }
    int  get_right() const { return db[right_id_].toInt(); }

    void release();
    void close();

};

const int ball_count = 3;


