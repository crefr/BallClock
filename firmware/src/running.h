#include <Arduino.h>
#include <GyverNTP.h>
#include <Looper.h>
#include <RunningGFX.h>
#include <WiFiConnector.h>
#include <ESP32Servo.h>

#include <AutoOTA.h>

#include "config.h"
#include "matrix.h"
#include "settings.h"
#include "piezo_player.h"

static void runString(String str, uint32_t color = 0x00ff00, uint16_t speed = 10) {
    RunningGFX run(matrix);
    matrix.clear();
    matrix.setModeDiag();
    run.setSpeed(speed);
    run.setWindow(0, matrix.width(), 1);
    run.setColor24(color);
    run.setFont(gfx_font_3x5);
    run.setText(str);
    run.start();

    while (run.tick() != RG_FINISH) {
        delay(0);
        yield();
    }
}
