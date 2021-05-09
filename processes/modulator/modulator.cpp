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


void modulator::receiveSamples() {
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

    modulate();
}

void modulator::modulate() {
    for (int i = 0; i < BUFFSIZE; i++) {
        //put sample data into shared memory
        char toMod = *(samplesToModify + i);

        *(modifiedSamples + i) = toMod;
        //*(sample + i) = (*(sample+i))+10;
    }
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


    modulator Modulator;

    int loops = 6897;

    while (loops > 0) {

        std::cout << std::endl << loops << std::endl;

        Modulator.receiveSamples();

        // change sample data
        loops--;

        Modulator.sendModulated();

    }
    return 0;
}

