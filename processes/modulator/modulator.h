//
// Created by laura on 01.05.2021.
//

#ifndef SCZR_PROJEKT_MODULATOR_H
#define SCZR_PROJEKT_MODULATOR_H

class modulator {

    boost::interprocess::message_queue *prodMod_mq;
    boost::interprocess::message_queue *modProd_mq;
    boost::interprocess::message_queue *modCons_mq;
    boost::interprocess::message_queue *consMod_mq;

    int messageBuffer[1]{};
    boost::interprocess::message_queue::size_type recvd_size{};
    unsigned int priority{};

    int sampleToModifyIndex{};
    int spaceToWriteIndex{};
    char *samplesToModify{};
    char *modifiedSamples{};

    char *unmodifiedSamples{};


public :

    void saveUnmodified();

    boost::interprocess::managed_shared_memory  *shMemory;

    modulator();
    ~modulator();

    void receiveSamples(double volumeChange);
    void modulate(double change);
    void sendModulated();
};

#endif //SCZR_PROJEKT_MODULATOR_H
