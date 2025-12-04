#pragma once

#include <Arduino.h>
#include "core/pos.h"
#include "matrix.h"

const int16_t snake_height = MX_DIAG_H;
const int16_t snake_width  = MX_DIAG_W;

enum class SnakeDest {
    Left, Right,
    UpRight, DownLeft,
    UpLeft, DownRight,
};

struct Snake {
    struct point {
        int16_t x;
        int16_t y;

        bool operator==(const point &rhs) const {
            return x == rhs.x && y == rhs.y;
        }
    };

    point snake[128];     // indexes of snake body
    point apple;
    uint8_t len;

    uint8_t speed;
    uint8_t cur_step = 0;

    SnakeDest new_dest;

    Snake(uint8_t speed_) : speed(speed_) {
        init_snake();
    }

    void tick ();

    void init_snake() {
        snake[0] = {3, 0};
        snake[1] = {2, 0};
        snake[2] = {1, 0};
        snake[3] = {0, 0};

        len = 4;

        dest = SnakeDest::Right;

        place_apple();
    }

    void draw (uint8_t hue);

    void set_dest_by_joystick(const sets::Pos &pos);

    void place_apple();

private:
    SnakeDest dest;

    void step ();
};

extern Snake snake;
