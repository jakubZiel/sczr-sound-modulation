//
// Created by laura on 01.05.2021.
//

#ifndef SCZR_PROJEKT_PRODUCER_H
#define SCZR_PROJEKT_PRODUCER_H

#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <sound/soundModule.h>


class producer {
    boost::interprocess::message_queue *modProd_mq{};
    boost::interprocess::message_queue *prodMod_mq{};

    int messageBuffer[1];
    boost::interprocess::message_queue::size_type recvd_size;
    unsigned int priority;

    soundModule alsa;

    char prodBuffer[BUFFSIZE];

    char *sample;

    void init();

public :
    producer();
    ~producer();

    void receiveSamples();
    void writeSamples();

    int getAlsaVal();
};



#endif //SCZR_PROJEKT_PRODUCER_H
