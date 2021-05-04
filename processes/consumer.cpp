//
// Created by laura on 30.04.2021.
//

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "consumer.h"
#include "utilities.h"

#include <boost/interprocess/managed_shared_memory.hpp>
#include <supervisor/supervisor.h>

using namespace boost::interprocess;

void consumer(){

    //TODO binding to CPU

    //get access to queues
    message_queue modCons_mq(open_only, "modifierConsumer_mq");
    message_queue consMod_mq(open_only, "consumerModifier_mq");

    int messageBuffer[1];
    message_queue::size_type recvd_size;
    unsigned int priority;
    while( !modCons_mq.try_receive(&messageBuffer[0], sizeof(int), recvd_size, priority));

    std::cout << "nastepna probka do wyslania do karty: " << messageBuffer[0] << std::endl;

    //get access to shared memory
    managed_shared_memory shMemory(open_only, "SoundBufferMemory");
    int *sample = shMemory.find<int>("modifierConsumerBuffer").first;
 //   int mcbsize = shMemory.get_instance_length("producerModifierBuffer");

    std::cout << "Consumer. Data received: " << std::endl;
    displaySample(sample, BUFFSIZE);

    //get samples from memory into buffer
    int consBuffer[BUFFSIZE];

    std::cout << "Consumer. Data from sample put into buffer: " << std::endl;
    for (int j=0; j<BUFFSIZE; j++) {
        consBuffer[j] = *(sample + j);
        std::cout << consBuffer[j] << " ";
    }

    while (!consMod_mq.try_send(&messageBuffer[0], sizeof (int), 0));

    std::cout << std::endl;
    std::cout << "ile mess w modCons? " << modCons_mq.get_num_msg() << std::endl;
    std::cout << "ile mess w consMod? " << consMod_mq.get_num_msg() << std::endl;

    //TODO put changed samples to alsa buffer



}