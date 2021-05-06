//
// Created by laura on 30.04.2021.
//

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "utilities.h"

#include <boost/interprocess/managed_shared_memory.hpp>
#include <supervisor/supervisor.h>

using namespace boost::interprocess;

int main(int argc, char *argv[]){

    //TODO binding to CPU

    //get access to queues
    message_queue prodMod_mq(open_only, "producerModifier_mq");
    message_queue modProd_mq(open_only, "modifierProducer_mq");

    message_queue modCons_mq(open_only, "modifierConsumer_mq");
    message_queue consMod_mq(open_only, "consumerModifier_mq");

    //check queues
    std::cout << "ile mess w prodMod? " << prodMod_mq.get_num_msg() << std::endl;
    std::cout << "ile mess w modProd? " << modProd_mq.get_num_msg() << std::endl;
    std::cout << "ile mess w modCons? " << modCons_mq.get_num_msg() << std::endl;
    std::cout << "ile mess w consMod? " << consMod_mq.get_num_msg() << std::endl;

    //is there free space to store modified sample?
    int messageBuffer[1];
    message_queue::size_type recvd_size;
    unsigned int priority;


    while (true) {
        while (!consMod_mq.try_receive(&messageBuffer[0], sizeof(int), recvd_size, priority));
        int modifiedIndex = messageBuffer[0];
        std::cout << "miejsce dla nastepnej probki: " << modifiedIndex << std::endl;

        while (!prodMod_mq.try_receive(&messageBuffer[0], sizeof(int), recvd_size, priority));
        int nextSampleToModifyIndex = messageBuffer[0];
        std::cout << "nastepna probka do modyfikacji: " << nextSampleToModifyIndex << std::endl;


        //get access to sample in shared memory
        managed_shared_memory shMemory(open_only, "SoundBufferMemory");
        int *samplesToModify = shMemory.find<int>("producerModifierBuffer").first;
        samplesToModify += nextSampleToModifyIndex * BUFFSIZE;
//    int pmbsize = shMemory.get_instance_length("producerModifierBuffer");

        //get access to place for modified sample
        int *modifiedSamples = shMemory.find<int>("modifierConsumerBuffer").first;
        //   int mcbsize = shMemory.get_instance_length("modifierConsumerBuffer");
        modifiedSamples += modifiedIndex * BUFFSIZE;

        std::cout << "Modulator. Data received: " << std::endl;
        displaySample(samplesToModify, BUFFSIZE);

        // change sample data
        for (int i = 0; i < BUFFSIZE; i++) {
            //put sample data into shared memory
            int toMod = *(samplesToModify + i);
            *(modifiedSamples + i) = toMod * 10;
            //*(sample + i) = (*(sample+i))+10;
        }

        messageBuffer[0] = nextSampleToModifyIndex;
        while (!modProd_mq.try_send(&messageBuffer[0], sizeof(int), 0));

        messageBuffer[0] = modifiedIndex;
        while (!modCons_mq.try_send(&messageBuffer[0], sizeof(int), 0));

        std::cout << "Modulator. Data changed: " << std::endl;
        displaySample(modifiedSamples, BUFFSIZE);

        std::cout << "ile mess w prodMod? " << prodMod_mq.get_num_msg() << std::endl;
        std::cout << "ile mess w modProd? " << modProd_mq.get_num_msg() << std::endl;
        std::cout << "ile mess w modCons? " << modCons_mq.get_num_msg() << std::endl;
        std::cout << "ile mess w consMod? " << consMod_mq.get_num_msg() << std::endl;

        std::cout << std::endl;

    }

    return 0;
}

