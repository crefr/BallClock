#include "snake.h"

#include <Arduino.h>
#include <FastLED.h>
#include <GyverNTP.h>
#include <Looper.h>
#include "esp_random.h"

#include "core/pos.h"
#include "matrix.h"

#define SQRT3 1.73

static void normalize(Snake::point &point);

void Snake::set_dest_by_joystick(const sets::Pos &pos) {
    float x = pos.x;
    float y = pos.y;

    if (x >= 0) {
        if      (x >= SQRT3 * y && x >= -SQRT3 * y) {
            if (dest != SnakeDest::Left) dest = SnakeDest::Right;
        } else if (y > 0) {
            if (dest != SnakeDest::UpLeft) dest = SnakeDest::DownRight;
        } else {
            if (dest != SnakeDest::DownLeft) dest = SnakeDest::UpRight;
        }
    } else {
        if      (x <= SQRT3 * y && x <= -SQRT3 * y) {
            if (dest != SnakeDest::Right) dest = SnakeDest::Left;
        } else if (y > 0) {
            if (dest != SnakeDest::UpRight) dest = SnakeDest::DownLeft;
        } else {
            if (dest != SnakeDest::DownRight) dest = SnakeDest::UpLeft;
        }
    }
}

void Snake::tick () {
    cur_step++;
    if (cur_step != speed) {
        return;
    }

    cur_step = 0;

    for (uint8_t snake_idx = len; snake_idx > 0; snake_idx--) {
        snake[snake_idx] = snake[snake_idx - 1];
    }

    step();

    for (uint8_t snake_idx = len - 1; snake_idx > 0; snake_idx--) {
        if (snake[snake_idx] == snake[0]) {
            // GAME OVER!!!
            init_snake();
        }
    }

    if (snake[0] == apple) {
        len++;
        place_apple();
    }
}

void Snake::draw (uint32_t color) {
    for (uint8_t snake_idx = 0; snake_idx < len; snake_idx++) {
        matrix.setLED(snake[snake_idx].x, snake[snake_idx].y, color);
    }

    matrix.setLED(apple.x, apple.y, 0xffffffff);
}

void Snake::place_apple() {
    int apple_pos = esp_random() % (128 - len);

    int pos = -1;
    int real_pos = 0;
    int x = 0; int y = 0;
    while (apple_pos != pos) {
        x = real_pos % 20;
        y = real_pos / 20;

        if (matrix.ledDiag(x, y) == -1) {
            real_pos++;
            continue;
        }

        pos++;
        for (uint8_t snake_idx = 0; snake_idx < len; snake_idx++) {
            if (snake[snake_idx].x == x
             && snake[snake_idx].y == y)  {
                pos--;
                break;
            }
        }

        real_pos++;
    }
    apple.x = x;
    apple.y = y;
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
        case SnakeDest::UpLeft: y++; break;
        case SnakeDest::DownRight: y--; break;

        case SnakeDest::UpRight: x++; y++; break;
        case SnakeDest::DownLeft: x--; y--; break;

        case SnakeDest::Right: x++; break;
        case SnakeDest::Left: x--; break;
    }

    normalize(snake[0]);

    if (matrix.ledDiag(x, y) == -1) {
        if (x <= 2) {
            switch (dest) {
                case SnakeDest::Left:  x = snake_width - 1; break;
                case SnakeDest::Right: x = (y - 4) + 1;     break;
                case SnakeDest::UpLeft:  y = 0;                 break;
                case SnakeDest::DownRight:    y = x + 3;         break;
            }
        } else {
            switch (dest) {
                case SnakeDest::Left:  x = y + snake_width - 4; break;
                case SnakeDest::Right: x = 0;                   break;
                case SnakeDest::UpLeft:  y = x - (snake_width - 4); break;
                case SnakeDest::DownRight:    y = snake_height - 1;  break;
            }
        }
    }
}

Snake snake{4};
