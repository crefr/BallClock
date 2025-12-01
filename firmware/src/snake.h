#pragma once

#include <Arduino.h>
#include "matrix.h"

const int16_t snake_height = MX_DIAG_H;
const int16_t snake_width  = MX_DIAG_W;

enum class SnakeDest {
    Up, Down, Left, Right
};

struct Snake {
    struct point {
        int16_t x;
        int16_t y;
    };

    point snake[128];     // indexes of snake body
    uint8_t len;

    SnakeDest dest;

    Snake() {
        snake[0] = {3, 0};
        snake[1] = {2, 0};
        snake[2] = {1, 0};
        len = 3;

        dest = SnakeDest::Right;
    }

    void tick ();

    void draw (uint32_t color);

    void set_dest (SnakeDest dest);

private:
    void step ();
};


