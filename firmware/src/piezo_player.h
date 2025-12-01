#pragma once

struct piezo_song {
    const int *melody;
    const float *note_durations;
    size_t len;

    float bpm;
};

extern piezo_song NeverGUp, Home;

struct piezo_player {
    int current_note = 0;
    int current_duration = 0;
    int current_note_pause = 0;

    long time = 0;
    bool playing = false;
    bool loop = true;

    piezo_song current_song = Home;

    void setup();
    void play();
    void tick();
    void stop();
};

extern piezo_player pieza;
