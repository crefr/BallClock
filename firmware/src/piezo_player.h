#pragma once

struct piezo_player {
    int current_note = 0;
    int current_duration = 0;
    int current_note_pause = 0;

    long time = 0;
    bool playing = false;

    void setup();
    void play();
    void loop();
    void stop();
};

extern piezo_player pieza;
