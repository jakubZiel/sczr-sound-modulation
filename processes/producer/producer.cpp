//
// Created by laura on 30.04.2021.
//
#include "processes/utilities.h"
#include "processes/consumer/producer.h"

#include <iostream>
#include <boost/interprocess/managed_shared_memory.hpp>
#include "sound/soundModule.h"
#include <boost/interprocess/sync/named_semaphore.hpp>


using namespace boost::interprocess;

int main(int argc, char *argv[])
{

    int wait;

    std::cin >> wait;

    message_queue modProd_mq(open_only, "modifierProducer_mq");
    message_queue prodMod_mq(open_only, "producerModifier_mq");

    int messageBuffer[1];
    message_queue::size_type recvd_size;
    unsigned int priority;

    managed_shared_memory shMemory(open_only, "SoundBufferMemory");

    soundModule alsa;

    alsa.openAlsa(RECORD);

    char prodBuffer[BUFFSIZE];

    int loops = (int) (5000000 / alsa.getVal());

    while (loops > 0) {
        //TODO get samples from alsa into buffer

        std::cout << loops << std::endl;


        alsa.recordSample(prodBuffer);

        std::cout << "recv | mq mod-prod : " << modProd_mq.get_num_msg() << std::endl;
        modProd_mq.receive(&messageBuffer[0], sizeof(int), recvd_size, priority);

        char *sample = shMemory.find<char>("producerModifierBuffer").first;

        sample = sample + messageBuffer[0] * BUFFSIZE;

        for (int j = 0; j < BUFFSIZE; j++) {
            //put sample data into shared memory
            *(sample + j) = prodBuffer[j];
        }

        std::cout << "send | mq prod-mod : " << prodMod_mq.get_num_msg() << std::endl;

        prodMod_mq.send(&messageBuffer[0], sizeof(int), 0);


        loops--;
    }
    return 0;
}