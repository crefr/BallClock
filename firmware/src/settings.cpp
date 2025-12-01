#include "settings.h"

#include <GyverNTP.h>
#include <LittleFS.h>
#include <Looper.h>
#include <SettingsESP.h>
#include <WiFiConnector.h>
#include <AutoOTA.h>

#include "config.h"
#include "core/containers.h"
#include "core/packet.h"
#include "palettes.h"
#include "redraw.h"

#include "servo_handler.h"
#include "piezo_player.h"

AutoOTA ota("", "");

GyverDBFile db(&LittleFS, "/data.db");
static SettingsESP sett(PROJECT_NAME " v" PROJECT_VER, &db);

extern ball_info balls[];

static sets::Pos stick_pos;

static void update(sets::Updater& u) {
    String s;
    s += photo.getRaw();
    s += " [";
    s += db[kk::adc_min].toInt16();
    s += ", ";
    s += db[kk::adc_max].toInt16();
    s += "]";
    u.update("adc_val"_h, s);

    s = String(stick_pos.x) + " " + String(stick_pos.y);
    u.update("joystick_coord"_h, s);

    s = String("hall2 = ") + String(analogRead(HALL_2)) + String("hall1 = ") + String(analogRead(HALL_1));
    u.update("holl1"_h, s);

    u.update("local_time"_h, NTP.timeToString());
    u.update("synced"_h, NTP.synced());

    Looper.getTimer("redraw")->restart(100);
}

static void build(sets::Builder& b) {
    {
        sets::Group g(b, "Режим");

        b.Select(kk::mode, "Режим", "Часы;Змейка");
    }
    {
        sets::Group g(b, "Часы");

        b.Select(kk::clock_style, "Шрифт", "Нет;Тип 1;Тип 2;Тип 3");
        b.Color(kk::clock_color, "Цвет");
    }
    /* ===================== Летающие шарики ===================== */
    static float left_angle[ball_count] = {db[kk::ball1_slider_left], db[kk::ball2_slider_left], db[kk::ball3_slider_left]};
    static float right_angle[ball_count] = {db[kk::ball1_slider_right], db[kk::ball2_slider_right], db[kk::ball3_slider_right]};

    {
        sets::Group g(b, "Лётчики");

        b.Label("---- Uno ----");
        b.Slider2("Угол", 0, 180, 1, Text(), &left_angle[0], &right_angle[0]);

        if (b.Switch(balls[0].get_id(), "Закрыть")) {
            Looper.pushEvent("update_servo");
        }

        b.Label("---- Dos ----");
        b.Slider2("Угол", 0, 180, 1, Text(), &left_angle[1], &right_angle[1]);

        if (b.Switch(balls[1].get_id(), "Закрыть")) {
            Looper.pushEvent("update_servo");
        }

        b.Label("---- Tres ----");
        b.Slider2("Угол", 0, 180, 1, Text(), &left_angle[2], &right_angle[2]);

        if (b.Switch(balls[2].get_id(), "Закрыть")) {
            Looper.pushEvent("update_servo");
        }

        if (b.Button("Сохранить")) {
            for (int i = 0; i < ball_count; i++) {
                balls[i].set_limits(left_angle[i], right_angle[i]);
            }
            Looper.pushEvent("update_servo");
        }

        b.Label("holl1"_h, "val = ");
    }
    /* ============================= Будильник ========================= */
    {
        sets::Group g(b, "Будильник");

        b.Time(kk::alarm_time, "Время");

        if (b.Button("play"_h, "Проиграть")) {
            pieza.play();
        }

        if (b.Button("stop"_h, "Стоп")) {
            pieza.stop();
        }
    }
    /* ============================= Змейка  =========================== */
    {
        sets::Group g(b, "Snake game");

        b.Joystick(stick_pos);
        b.Label("joystick_coord"_h, String(stick_pos.x) + " " + String(stick_pos.y));
    }
    {
        sets::Group g(b, "Фон");

        if (b.Select(kk::back_mode, "Фон", "Нет;Градиент;Перлин")) b.reload();

        if (db[kk::back_mode].toInt()) {
            b.Select(kk::back_pal, "Палитра", getPaletteList());
            b.Slider(kk::back_bright, "Яркость", 0, 255);
            b.Slider(kk::back_speed, "Скорость");
            b.Slider(kk::back_scale, "Масштаб");
            b.Slider(kk::back_angle, "Угол", -180, 180);
        }
    }
    {
        sets::Group g(b, "Яркость");
        if (b.Switch(kk::auto_bright, "Автояркость")) b.reload();
        b.Label("adc_val"_h, "Сигнал с датчика");

        if (db[kk::auto_bright]) {
            b.Slider(kk::bright_min, "Мин.", 0, 255);
            b.Slider(kk::bright_max, "Макс.", 0, 255);

            {
                sets::Buttons bt(b);
                if (b.Button(kk::adc_min, "Запомнить мин.")) db[kk::adc_min] = photo.getRaw();
                if (b.Button(kk::adc_max, "Запомнить макс.")) db[kk::adc_max] = photo.getRaw();
            }
        } else {
            b.Slider(kk::bright, "Яркость", 0, 255);
        }
    }
    {
        sets::Group g(b, "Ночной режим");

        if (b.Switch(kk::night_mode, "Включен")) b.reload();

        if (db[kk::night_mode]) {
            b.Color(kk::night_color, "Цвет");
            b.Slider(kk::night_trsh, "Порог", 0, 1023);
        }
    }
    {
        sets::Group g(b, "Время");

        b.Input(kk::ntp_gmt, "Часовой пояс");
        b.Input(kk::ntp_host, "NTP сервер");
        b.LED("synced"_h, "Синхронизирован", NTP.synced());
        b.Label("local_time"_h, "Локальное время", NTP.timeToString());
    }
    {
        sets::Group g(b, "WiFi");

        b.Switch(kk::show_ip, "Показывать IP");
        b.Input(kk::wifi_ssid, "SSID");
        b.Pass(kk::wifi_pass, "Pass", "");

        if (b.Button("wifi_save"_h, "Подключить")) {
            Looper.pushEvent("wifi_connect");
        }
    }

    if (b.build.isAction()) {
        switch (b.build.id) {
            case kk::ntp_gmt: NTP.setGMT(b.build.value); break;
            case kk::ntp_host: NTP.setHost(b.build.value); break;
        }
    }

    Looper.getTimer("redraw")->restart(100);
    // if (b.Button("restart"_h, "restart")) ESP.restart();
}



LP_LISTENER_("wifi_connect", []() {
    db.update();
    WiFiConnector.connect(db[kk::wifi_ssid], db[kk::wifi_pass]);
});

LP_TICKER([]() {
    if (Looper.thisSetup()) {
        LittleFS.begin(true);
        db.begin();

        db.init(kk::wifi_ssid, "");
        db.init(kk::wifi_pass, "");
        db.init(kk::show_ip, true);

        db.init(kk::ntp_host, "pool.ntp.org");
        db.init(kk::ntp_gmt, 3);

        db.init(kk::mode, 0);

        db.init(kk::ball1_slider_left, 0);
        db.init(kk::ball1_slider_right, 0);
        db.init(kk::ball1, false);

        db.init(kk::ball2_slider_left, 0);
        db.init(kk::ball2_slider_right, 0);
        db.init(kk::ball2, false);

        db.init(kk::ball3_slider_left, 0);
        db.init(kk::ball3_slider_right, 0);
        db.init(kk::ball3, false);

        db.init(kk::bright, 100);
        db.init(kk::auto_bright, false);
        db.init(kk::bright_min, 10);
        db.init(kk::bright_max, 255);
        db.init(kk::adc_min, 0);
        db.init(kk::adc_max, 1023);

        db.init(kk::night_mode, false);
        db.init(kk::night_color, 0xff0000);
        db.init(kk::night_trsh, 50);

        db.init(kk::clock_style, 1);
        db.init(kk::clock_color, 0xffffff);

        db.init(kk::back_mode, 1);
        db.init(kk::back_pal, 0);
        db.init(kk::back_bright, 200);
        db.init(kk::back_speed, 50);
        db.init(kk::back_scale, 50);
        db.init(kk::back_angle, 0);

        // sett.config
        WiFiConnector.connect(db[kk::wifi_ssid], db[kk::wifi_pass]);
        sett.begin();
        sett.onBuild(build);
        sett.onUpdate(update);

        NTP.setHost(db[kk::ntp_host]);
        NTP.setGMT(db[kk::ntp_gmt]);
    }

    WiFiConnector.tick();
    sett.tick();
    ota.tick();
    NTP.tick();
});


// LP_TIMER(1000, []() {
//     Serial.println(ESP.getFreeHeap());
// });
