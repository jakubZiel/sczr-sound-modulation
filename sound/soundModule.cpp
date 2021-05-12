//
// Created by jakub on 07.05.2021.
//

#include "soundModule.h"

#include <alsa/asoundlib.h>
#include <iostream>


soundModule::soundModule() = default;

void soundModule::closeAlsa(char* buffer){
    snd_pcm_close(handle);
    free(buffer);
}

void soundModule::openAlsa(int mode){
    /* Open PCM device for playback. */


    if (mode == PLAYBACK)
        rc = snd_pcm_open(&handle, "default",SND_PCM_STREAM_PLAYBACK, 0);
    else if (mode == RECORD){
        rc = snd_pcm_open(&handle, "default",SND_PCM_STREAM_CAPTURE, 0);

    }


    if (rc < 0) {
        fprintf(stderr,
                "unable to open pcm device: %s\n",
                snd_strerror(rc));
        exit(1);
    }

    /* Allocate a hardware parameters object. */
    snd_pcm_hw_params_alloca(&params);

    /* Fill it in with default values. */
    snd_pcm_hw_params_any(handle, params);

    /* Set the desired hardware parameters. */

    /* Interleaved mode */
    snd_pcm_hw_params_set_access(handle, params,
                                 SND_PCM_ACCESS_RW_INTERLEAVED);

    /* Signed 16-bit little-endian format */
    snd_pcm_hw_params_set_format(handle, params,
                                 SND_PCM_FORMAT_S16_LE);

    /* Two channels (stereo) */
    snd_pcm_hw_params_set_channels(handle, params, 2);

    /* 44100 bits/second sampling rate (CD quality) */
    val = 44100;
    snd_pcm_hw_params_set_rate_near(handle, params,
                                    &val, &dir);

    /* Set period size to 32 frames. */
    frames = 32;
    snd_pcm_hw_params_set_period_size_near(handle,
                                           params, &frames, &dir);


    /* Write the parameters to the driver */
    rc = snd_pcm_hw_params(handle, params);
    if (rc < 0) {
        fprintf(stderr,
                "unable to set hw parameters: %s\n",
                snd_strerror(rc));
        exit(1);
    }

    /* Use a buffer large enough to hold one period */
    snd_pcm_hw_params_get_period_size(params,
                                      &frames, &dir);

    size = (int) frames * 4; /* 2 bytes/sample, 2 channels */

    snd_pcm_hw_params_get_period_time(params,
                                      &val, &dir);

}

void soundModule::playSample(int file_d, char* buffer) {


    rc = read(file_d, buffer, size);
    if (rc == 0) {
        fprintf(stderr, "end of file on input\n");
    } else if (rc != size) {
        fprintf(stderr,
                "short read: read %d bytes\n", rc);
    }
    rc = snd_pcm_writei(handle, buffer, frames);
    if (rc == -EPIPE) {

        fprintf(stderr, "underrun occurred\n");
        snd_pcm_prepare(handle);
    } else if (rc < 0) {
        fprintf(stderr,
                "error from writei: %s\n",
                snd_strerror(rc));
    }  else if (rc != (int)frames) {
        fprintf(stderr,
                "short write, write %d frames\n", rc);
    }
}


void soundModule::play(int howLong, char *inputFile) {



    int loops = (int) (howLong / val);

    int file_d = open(inputFile, O_RDONLY);

    char* buffer = (char*) malloc(size);

    while (loops > 0) {
        std::cout << loops << std::endl;
        loops--;
        playSample(file_d, buffer);
    }

    snd_pcm_drain(handle);
    snd_pcm_close(handle);
    free(buffer);

}


void soundModule::record(int howLong, char *outFile) {
    int loops = (int) (howLong / val);

    int file_d = open(outFile, O_WRONLY);

    char* buffer = (char*) malloc(size);

    while (loops > 0){
        std::cout << loops << std::endl;
        loops--;
        recordSample(buffer);

        writeSample(buffer, file_d);
    }

    snd_pcm_close(handle);
    free(buffer);
}


void soundModule::recordSample(char *buffer) {

    rc = snd_pcm_readi(handle, buffer, frames);
    if (rc == -EPIPE) {
        /* EPIPE means overrun */
        fprintf(stderr, "overrun occurred\n");
        snd_pcm_prepare(handle);
    } else if (rc < 0) {
        fprintf(stderr,
                "error from read: %s\n",
                snd_strerror(rc));
    } else if (rc != (int)frames) {
        fprintf(stderr, "short read, read %d frames\n", rc);
    }
}

void soundModule::writeSample(char* buffer, int file_d){
    rc = write(file_d, buffer, size);
    if (rc != size) {
        fprintf(stderr,
                "short write: wrote %d bytes errno = %d\n", rc, errno);
    }
}

unsigned int soundModule::getVal() const {
    return val;
}

size_t soundModule::getBufferSize() const {
    return size;
}
