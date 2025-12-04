#include "settings.h"

#include <GyverNTP.h>
#include <LittleFS.h>
#include <Looper.h>
#include <SettingsESP.h>
#include <WiFiConnector.h>
#include <AutoOTA.h>
#include <algorithm>
#include <cstring>

#include "RunningGFX.h"
#include "config.h"
#include "core/containers.h"
#include "core/packet.h"
#include "palettes.h"
#include "redraw.h"

#include "servo_handler.h"
#include "piezo_player.h"
#include "snake.h"
#include "alarm.h"

AutoOTA ota("", "");

GyverDBFile db(&LittleFS, "/data.db");
static SettingsESP sett(PROJECT_NAME " v" PROJECT_VER, &db);

extern ball_info balls[];

static sets::Pos stick_pos;

// running str
uint16_t str_speed = 10;
char running_str_chr[256] = "";
RunningGFX * running_str_ptr = nullptr;

static void update(sets::Updater& u) {
    String s;
    s += photo.getRaw();
    s += " [";
    s += db[kk::adc_min].toInt16();
    s += ", ";
    s += db[kk::adc_max].toInt16();
    s += "]";
    u.update("adc_val"_h, s);

    // s = String(stick_pos.x) + " " + String(stick_pos.y);
    // u.update("joystick_coord"_h, s);

    s = String("hall2 = ") + String(analogRead(HALL_2)) + String("hall1 = ") + String(analogRead(HALL_1));
    u.update("holl1"_h, s);

    u.update("local_time"_h, NTP.timeToString());
    u.update("synced"_h, NTP.synced());

    Looper.getTimer("redraw")->restart(100);
}

#define BUTTON_NAME "o\no\no"

static void build(sets::Builder& b) {
    {
        sets::Group g(b, "Режим");

        if (b.Select(kk::mode, "Режим", "Часы;Змейка;Бегущая строка")) {
            matrix.clear();
            b.reload();
        }
    }
    if (db[kk::mode].toInt() == 1) {
        /* ============================= Змейка  =========================== */
        sets::Group g(b, "Змейка");

        if (b.Select(kk::snake_control, "Управление", "Джойстик;Кнопки")) b.reload();

        if (db[kk::snake_control].toInt() == 0) {
            if (b.Joystick(stick_pos, true)) snake.set_dest_by_joystick(stick_pos);
            b.Label("joystick_coord"_h, String(stick_pos.x) + " " + String(stick_pos.y));
        } else {
            {
                sets::Buttons bt_row_1(b);
                if (b.Button(BUTTON_NAME)) snake.new_dest = SnakeDest::DownLeft;
                if (b.Button(BUTTON_NAME)) snake.new_dest = SnakeDest::DownRight;
            }
            {
                sets::Buttons bt_row_2(b);
                if (b.Button(BUTTON_NAME)) snake.new_dest = SnakeDest::Left;
                b.Button(" ");
                if (b.Button(BUTTON_NAME)) snake.new_dest = SnakeDest::Right;
            }
            {
                sets::Buttons bt_row_3(b);
                if (b.Button(BUTTON_NAME)) snake.new_dest = SnakeDest::UpLeft;
                if (b.Button(BUTTON_NAME)) snake.new_dest = SnakeDest::UpRight;
            }
        }
    } else if (db[kk::mode].toInt() == 2) {
    /* ============================= Бегущая строка  =========================== */
        sets::Group g(b, "Бегущая строка");

        b.Slider(kk::running_str_speed, "Замедление", 1, 10, 1);
        b.Input(kk::running_str, "Текст");

        if (b.Button("run_str_btn"_h, "Проиграть"))
            Looper.pushEvent("run_string");
    }
    {
        sets::Group g(b, "Часы");

        b.Select(kk::clock_style, "Шрифт", "Нет;Тип 1;Тип 2;Тип 3");
        b.Color(kk::clock_color, "Цвет");
    }
    /* ===================== Летающие шарики ===================== */
    static float  left_angle[ball_count] = {db[kk::ball2_slider_left] , db[kk::ball3_slider_left] };
    static float right_angle[ball_count] = {db[kk::ball2_slider_right], db[kk::ball3_slider_right]};

    {
        sets::Group g(b, "Лётчики");

        b.Label("---- Left ----");
        b.Slider2("Угол", 0, 180, 1, Text(), &left_angle[0], &right_angle[0]);

        if (b.Switch(balls[0].get_id(), "Закрыть")) {
            Looper.pushEvent("update_servo");
        }

        b.Label("---- Right ----");
        b.Slider2("Угол", 0, 180, 1, Text(), &left_angle[1], &right_angle[1]);

        if (b.Switch(balls[1].get_id(), "Закрыть")) {
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

        if (b.Switch(kk::alarm_on, "Включить")) {
            clock_alarm.set_state(db[kk::alarm_on].toBool());
        }

        if (b.Time(kk::alarm_time, "Время")) {
            Serial.printf("Setting alarm time to %d\n", db[kk::alarm_time]);
            clock_alarm.set(db[kk::alarm_time].toInt());
        }

        if (b.Select(kk::alarm_song, "Мелодия будильника", "1;2")) {
            pieza.current_song = alarm_melodies[db[kk::alarm_song]];
        }

        if (b.Button("play"_h, "Проиграть")) {
            pieza.play();
        }

        if (b.Button("stop"_h, "Стоп")) {
            pieza.stop();
        }
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

LP_LISTENER_("run_string", []() {
    db.update();

    running_str_ptr->stop();
    running_str_ptr->setColor24(matrix.getColor24());
    running_str_ptr->setFont(gfx_font_3x5);

    String input = db[kk::running_str].toText();
    strncpy(running_str_chr, input.c_str(), 255);
    running_str_ptr->setText(running_str_chr);
    running_str_ptr->start();
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
        db.init(kk::snake_control, 0);
        db.init(kk::running_str, "string");
        db.init(kk::running_str_speed, 1);

        db.init(kk::ball2_slider_left, 0);
        db.init(kk::ball2_slider_right, 0);
        db.init(kk::ball2, false);

        db.init(kk::ball3_slider_left, 0);
        db.init(kk::ball3_slider_right, 0);
        db.init(kk::ball3, false);

        db.init(kk::alarm_on, false);
        db.init(kk::alarm_time, 0);
        db.init(kk::alarm_song, 0);

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

        WiFiConnector.connect(db[kk::wifi_ssid], db[kk::wifi_pass]);
        sett.begin();
        sett.onBuild(build);
        sett.onUpdate(update);

        NTP.setHost(db[kk::ntp_host]);
        NTP.setGMT(db[kk::ntp_gmt]);

        running_str_ptr = new RunningGFX(matrix);
        running_str_ptr->setWindow(0, matrix.width(), 1);
        running_str_ptr->setColor24(0x00ff00);
        running_str_ptr->setFont(gfx_font_3x5);

        clock_alarm.set(db[kk::alarm_time].toInt());
        clock_alarm.set_state(db[kk::alarm_on].toBool());
    }

    WiFiConnector.tick();
    sett.tick();
    ota.tick();
    NTP.tick();
    clock_alarm.tick();
});


// LP_TIMER(1000, []() {
//     Serial.println(ESP.getFreeHeap());
// });
