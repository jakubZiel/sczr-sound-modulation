//
// Created by jakub on 07.05.2021.
//

#ifndef SCZR_SOUND_MOD_SOUNDMODULE_H
#define SCZR_SOUND_MOD_SOUNDMODULE_H

#include <alsa/asoundlib.h>

#define PLAYBACK 0
#define RECORD 1

class soundModule {

    int rc{};
    int size{};
    snd_pcm_t *handle{};
    snd_pcm_hw_params_t *params{};
    unsigned int val{};
    int dir{};
    snd_pcm_uframes_t frames{};




public:

    unsigned int getVal() const;
    size_t getBufferSize() const;

    void record(int howLong, char* outFile);
    void recordSample(char* buffer);
    void writeSample(char *buffer, int file_d);

    void play(int howLong, char* inputFile);
    void playSample(int file_d, char* buffer);

    void openAlsa(int mode);
    void closeAlsa(char *buffer);

    soundModule();

};


#endif //SCZR_SOUND_MOD_SOUNDMODULE_H
