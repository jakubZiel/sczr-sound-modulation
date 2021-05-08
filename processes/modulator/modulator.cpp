//
// Created by laura on 30.04.2021.
//

#include <iostream>

#include "processes/utilities.h"
#include <supervisor/supervisor.h>
#include <boost/interprocess/sync/named_semaphore.hpp>
#include <pthread.h>

using namespace boost::interprocess;

int main(int argc, char *argv[]){


    //get access to queues
    message_queue prodMod_mq(open_only, "producerModifier_mq");
    message_queue modProd_mq(open_only, "modifierProducer_mq");

    message_queue modCons_mq(open_only, "modifierConsumer_mq");
    message_queue consMod_mq(open_only, "consumerModifier_mq");


    //is there free space to store modified sample?
    int messageBuffer[1];
    message_queue::size_type recvd_size;
    unsigned int priority;
    int loops = 6897;


    while (loops > 0) {

        std::cout << loops << std::endl;

        std::cout << "recv | mq cons-mod : " << consMod_mq.get_num_msg() << std::endl;

        consMod_mq.receive(&messageBuffer[0], sizeof(int), recvd_size, priority);
        int spaceToWriteIndex = messageBuffer[0];

        std::cout << "recv | mq prod-mod : " << prodMod_mq.get_num_msg() << std::endl;

        prodMod_mq.receive(&messageBuffer[0], sizeof(int), recvd_size, priority);
        int sampleToModifyIndex = messageBuffer[0];


        //get access to sample in shared memory
        managed_shared_memory shMemory(open_only, "SoundBufferMemory");

        char *samplesToModify = shMemory.find<char>("producerModifierBuffer").first;
        samplesToModify += sampleToModifyIndex * BUFFSIZE;

        char *modifiedSamples = shMemory.find<char>("modifierConsumerBuffer").first;
        modifiedSamples += spaceToWriteIndex * BUFFSIZE;

        // change sample data
        //*
        for (int i = 0; i < BUFFSIZE; i++) {
            //put sample data into shared memory
            char toMod = *(samplesToModify + i);
            *(modifiedSamples + i) = toMod;
            //*(sample + i) = (*(sample+i))+10;
        }

        loops--;

        std::cout << "send | mq mod-prod : " << modProd_mq.get_num_msg() << std::endl;

        messageBuffer[0] = sampleToModifyIndex;
        modProd_mq.send(&messageBuffer[0], sizeof(int), 0);

        std::cout << "send | mq mod-cons : " << modCons_mq.get_num_msg() << std::endl;

        messageBuffer[0] = spaceToWriteIndex;
        modCons_mq.send(&messageBuffer[0], sizeof(int), 0);

    }
    return 0;
}

