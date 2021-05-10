//
// Created by jakub on 07.05.2021.
//
#include "sound/soundModule.h"

int main() {
    soundModule alsa;

    alsa.openAlsa(PLAYBACK);
    alsa.play(10000000, (char*) "out.wav");
}
