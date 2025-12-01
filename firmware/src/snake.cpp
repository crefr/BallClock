#include "snake.h"

#include <Arduino.h>
#include <FastLED.h>
#include <GyverNTP.h>
#include <Looper.h>

#include "matrix.h"

static void normalize(Snake::point &point);

void Snake::set_dest(SnakeDest destination) {
    dest = destination;
}

void Snake::tick () {
    for (uint8_t snake_idx = len - 1; snake_idx > 0; snake_idx--) {
        snake[snake_idx] = snake[snake_idx - 1];
    }

    step();
}

void Snake::draw (uint32_t color) {
    for (uint8_t snake_idx = 0; snake_idx < len; snake_idx++) {
        matrix.setLED(snake[snake_idx].x, snake[snake_idx].y, color);
    }
}

static void normalize(Snake::point &point) {
    if      (point.x > snake_width - 1) point.x = 0;
    else if (point.x < 0)               point.x = snake_width - 1;

    if      (point.y > snake_height - 1) point.y = 0;
    else if (point.y < 0)                point.y = snake_height - 1;
}

void Snake::step() {
    int16_t &x = snake[0].x;
    int16_t &y = snake[0].y;

    switch(dest) {
        case SnakeDest::Up:
            y++;
            break;
        case SnakeDest::Down:
            y--;
            break;

        case SnakeDest::Right:
            x++;
            break;
        case SnakeDest::Left:
            x--;
            break;
    }

    normalize(snake[0]);

    for (int i = 0; i < 3; i++) {
        if (y == (4 + i) && (0 <= x && x <= i)) {
            if      (dest == SnakeDest::Left)
                x = snake_width - 1;
            else if (dest == SnakeDest::Right)
                x = i + 1;
            else if (dest == SnakeDest::Down)
                y = 0;
            else // (dest == SnakeDest::Up)
                y = 3 + i;

            break;
        }
    }

    for (int i = 0; i < 3; i++) {
        if (y == i && snake_width - 3 <= x && x <= snake_width - 1) {
            if      (dest == SnakeDest::Left)
                x = snake_width - 4 + i;
            else if (dest == SnakeDest::Right)
                x = 0;
            else if (dest == SnakeDest::Down)
                y = i + 1;
            else // (dest == SnakeDest::Up)
                y = snake_height - 1;

            break;
        }
    }
}
