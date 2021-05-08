//
// Created by laura on 30.04.2021.
//
#include "processes/utilities.h"
#include "producer.h"

#include <iostream>
#include <boost/interprocess/managed_shared_memory.hpp>
#include "sound/soundModule.h"


using namespace boost::interprocess;

producer::producer() {
    init();
}

producer::~producer() {
    delete modProd_mq;
    delete prodMod_mq;
}

void producer::init() {

    modProd_mq = new message_queue(open_only, "modifierProducer_mq");
    prodMod_mq = new message_queue(open_only, "producerModifier_mq");

    alsa.openAlsa(RECORD);
}

void producer::receiveSamples() {

    managed_shared_memory shMemory(open_only, "SoundBufferMemory");
    alsa.recordSample(prodBuffer);

    std::cout << "recv | mq mod-prod : " << modProd_mq->get_num_msg() << std::endl;
    modProd_mq->receive(&messageBuffer[0], sizeof(int), recvd_size, priority);

    char *sample = shMemory.find<char>("producerModifierBuffer").first;

    sample = sample + messageBuffer[0] * BUFFSIZE;

    displaySampleChar(sample);
}

void producer::writeSamples() {
    for (int j = 0; j < BUFFSIZE; j++) {
        //put sample data into shared memory
        *(sample + j) = prodBuffer[j];
    }

    std::cout << "send | mq prod-mod : " << prodMod_mq->get_num_msg() << std::endl;
    prodMod_mq->send(&messageBuffer[0], sizeof(int), 0);
}

int producer::getAlsaVal() {
    return alsa.getVal();
}

int main(int argc, char *argv[])
{
    producer Producer;

    int wait;
    std::cin >> wait;

    int loops = (int) (5000000 / Producer.getAlsaVal());

    while (loops > 0) {

        Producer.receiveSamples();

        std::cout << loops << std::endl;

        Producer.writeSamples();

        loops--;
    }
    return 0;
}
