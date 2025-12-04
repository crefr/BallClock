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
#include "running.h"

Servo servo_left;
Servo servo_right;

void setup() {
    Serial.begin(115200);
    Serial.println("\n" PROJECT_NAME " v" PROJECT_VER);

    matrix.begin();

    ESP32PWM::allocateTimer(1);
    servo_left.attach(SERVO_2);
    servo_right.attach(SERVO_3);

    pieza.setup();

    WiFiConnector.setName(PROJECT_NAME);

    WiFiConnector.onConnect([]() {
        NTP.begin();

        Serial.print("Connected: ");
        Serial.println(WiFi.localIP());
        if (db[kk::show_ip]) runString(WiFi.localIP().toString());

        // ota.checkUpdate();
    });

    WiFiConnector.onError([]() {
        String str("Error! AP: ");
        str += WiFi.softAPSSID();
        Serial.println(str);
        runString(str);
    });
}

void loop() {
    Looper.loop();
}
