#pragma once

#include <GyverDBFile.h>
#include <AutoOTA.h>

extern GyverDBFile db;
extern AutoOTA ota;

DB_KEYS(
    kk,
    wifi_ssid,
    wifi_pass,
    show_ip,

    ntp_gmt,
    ntp_host,

    alarm_time,
    alarm_on,
    alarm_song,
    mode,       // 0 - clock, 1 - snaky

    ball1_slider_left,
    ball1_slider_right,
    ball1,

    ball2_slider_left,
    ball2_slider_right,
    ball2,

    ball3_slider_left,
    ball3_slider_right,
    ball3,

    bright,
    auto_bright,
    bright_min,
    bright_max,
    adc_min,
    adc_max,

    night_mode,
    night_color,
    night_trsh,

    clock_style,
    clock_color,

    back_mode,  // 0 none, 1 grad, 2 perlin
    back_pal,
    back_speed,
    back_angle,
    back_scale,
    back_bright

);
