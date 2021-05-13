//
// Created by laura on 30.04.2021.
//

#include <iostream>

#include "processes/utilities.h"
#include <supervisor/supervisor.h>
#include "modulator.h"

using namespace boost::interprocess;

modulator::modulator() {
    modProd_mq = new message_queue(open_only, "modifierProducer_mq");
    prodMod_mq = new message_queue(open_only, "producerModifier_mq");

    modCons_mq = new message_queue(open_only, "modifierConsumer_mq");
    consMod_mq = new message_queue(open_only, "consumerModifier_mq");

    shMemory = new managed_shared_memory (open_only, "SoundBufferMemory");


}

modulator::~modulator() {
    delete prodMod_mq;
    delete modProd_mq;
    delete modCons_mq;
    delete consMod_mq;
    delete shMemory;
}


void modulator::receiveSamples(double volumeChange) {
    std::cout << "recv | mq prod-mod : " << prodMod_mq->get_num_msg() << std::endl;
    prodMod_mq->receive(&messageBuffer[0], sizeof(int), recvd_size, priority);
    sampleToModifyIndex = messageBuffer[0];

    std::cout << "recv | mq cons-mod : " << consMod_mq->get_num_msg() << std::endl;
    consMod_mq->receive(&messageBuffer[0], sizeof(int), recvd_size, priority);
    spaceToWriteIndex = messageBuffer[0];


    //get access to sample in shared memory

    samplesToModify = shMemory->find<char>("producerModifierBuffer").first;
    samplesToModify += sampleToModifyIndex * BUFFSIZE;

    modifiedSamples = shMemory->find<char>("modifierConsumerBuffer").first;
    modifiedSamples += spaceToWriteIndex * BUFFSIZE;


    unmodifiedSamples = shMemory->find<char>("unmodifiedBuffer").first;
    unmodifiedSamples += spaceToWriteIndex * BUFFSIZE;

    modulate(volumeChange);

    saveUnmodified();
}

void modulator::saveUnmodified() {
    memcpy(unmodifiedSamples, samplesToModify, BUFFSIZE);
}

void modulator::modulate(double volumeChange) {
    for (int i = 0; i < BUFFSIZE; i++) {
        int16_t toMod = *(samplesToModify + i);

        toMod *= volumeChange;
        //put sample data into shared memory

        *(modifiedSamples + i) = toMod;
    }

    //artificial cpu cycles only used during performance testing
    for (int i = 0; i < 500000; i++);
}

void modulator::sendModulated() {
    std::cout << "send | mq mod-prod : " << modProd_mq->get_num_msg() << std::endl;

    messageBuffer[0] = sampleToModifyIndex;
    modProd_mq->send(&messageBuffer[0], sizeof(int), 0);

    std::cout << "send | mq mod-cons : " << modCons_mq->get_num_msg() << std::endl;

    messageBuffer[0] = spaceToWriteIndex;
    modCons_mq->send(&messageBuffer[0], sizeof(int), 0);
}

int main(int argc, char *argv[]){

    /* only used during performance testing
    struct sched_param p;
    p.sched_priority = 99;


    sched_setscheduler(0 , SCHED_FIFO, &p);
    if (sched_getscheduler(0) != SCHED_FIFO)
        return -1;

    std::cout << "FIFO" << std::endl;
    */

    //testing purposes
    /*
    cpu_set_t cpuSet;
    CPU_ZERO(&cpuSet);
    CPU_SET(7, &cpuSet);
    CPU_SET(6, &cpuSet);
    CPU_SET(5, &cpuSet);
    CPU_SET(4, &cpuSet);

    if (sched_setaffinity(0, sizeof(cpuSet), &cpuSet) == -1)
        std::cout << "didn't set the cpu :" << errno << std::endl;
    else
        std::cout << "cpu set to :" << sched_getcpu() << std::endl;
    */
    modulator Modulator;

    std::cout << "modulator\n\n";

    named_semaphore input(open_only, "userInputSem");

    input.wait();

    int loops = *(Modulator.shMemory->find<int>("recordingTime").first);
    loops /= 725;

    double volumeChange = *(Modulator.shMemory->find<double>("volumeChange").first);

    while (loops > 0) {

        std::cout << std::endl << loops << std::endl;

        Modulator.receiveSamples(volumeChange);

        // change sample data
        loops--;

        if (loops == 1)
            break;

        Modulator.sendModulated();

    }
    return 0;
}

