//
// Created by laura on 30.04.2021.
//

#include <iostream>
#include "processes/utilities.h"

#include <supervisor/supervisor.h>
#include "consumer.h"
#include <fcntl.h>
#include "sound/soundModule.h"



using namespace boost::interprocess;

consumer::consumer(){
    init();

}

consumer::~consumer() {

    delete modCons_mq;
    delete consMod_mq;

    close(file_d);
}

void consumer::init() {
    //TODO should be like this, new object?
    modCons_mq = new message_queue(open_only, "modifierConsumer_mq");
    consMod_mq = new message_queue(open_only, "consumerModifier_mq");
    shMemory = new managed_shared_memory(open_only, "SoundBufferMemory");


    alsa.openAlsa(PLAYBACK);
    file_d = open("out.wav", O_WRONLY);
}

void consumer::receiveSamples() {
    std::cout << "recv | mq mod-cons : " << modCons_mq->get_num_msg() << std::endl;

    modCons_mq->receive(&messageBuffer[0], sizeof(int), recvd_size, priority);

    //get access to shared memory
    //TODO should be in constructor?

    sample = shMemory->find<char>("modifierConsumerBuffer").first;
    sample += messageBuffer[0] * BUFFSIZE;
}

void consumer::writeSamples() {
    alsa.writeSample(sample, file_d);

    //get samples from memory into buffer
    std::cout << "send | mq cons-mod : " << consMod_mq->get_num_msg() << std::endl;

    consMod_mq->send(&messageBuffer[0], sizeof(int), 0);
}

int main(int argc, char *argv[])
{

    consumer Consumer;
    //TODO binding to CPU

    int loops = 6897;

    while (loops > 0) {

        Consumer.receiveSamples();

        std::cout << "write sample" << 6897 - loops << std::endl;
        Consumer.writeSamples();

        loops--;
    }

    return 0;
}