//
// Created by laura on 01.05.2021.
//

#ifndef SCZR_PROJEKT_CONSUMER_H
#define SCZR_PROJEKT_CONSUMER_H

#include <sound/soundModule.h>
#include "measurement/measurementModule.h"

class consumer {
    boost::interprocess::message_queue *modCons_mq{};
    boost::interprocess::message_queue *consMod_mq{};
    boost::interprocess::managed_shared_memory *shMemory{};

    measurementModule* latencyRecorder{};

    int messageBuffer[1]{};
    boost::interprocess::message_queue::size_type recvd_size{};
    unsigned int priority{};
    int file_d{};
    int original_file_d{};

    soundModule alsa;

    char *sample{};
    char *originalSample{};

public :
    consumer();
    ~consumer();

    void receiveSamples();
    void writeSamples(int currSample);

    void writeToFile(char* file);
};

#endif //SCZR_PROJEKT_CONSUMER_H
