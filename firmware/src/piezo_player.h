#pragma once

struct piezo_player {
    int current_note = 0;
    int current_duration = 0;
    int current_note_pause = 0;

    long time = 0;
    bool playing = false;
    bool loop = true;

    void setup();
    void play();
    void tick();
    void stop();
};

extern piezo_player pieza;
