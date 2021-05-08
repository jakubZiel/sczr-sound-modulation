//
// Created by jakub on 07.05.2021.
//
#include "sound/soundModule.h"

int main() {
    soundModule alsa;

    //alsa.openAlsa(RECORD);

    //alsa.record(5000000, (char*) "out.wav");


    alsa.openAlsa(PLAYBACK);
    alsa.play(5000000, (char*) "out.wav");
}
