//
// Created by laura on 30.04.2021.
//
#include "utilities.h"
#include "producer.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include <boost/interprocess/managed_shared_memory.hpp>
#include <supervisor/supervisor.h>

using namespace boost::interprocess;

void producer(){

    //TODO binding to CPU

    //TODO get samples from alsa into buffer
    int prodBuffer[BUFFSIZE];
    for (int i=0; i<BUFFSIZE; i++)
        prodBuffer[i]=i;

    message_queue modProd_mq(open_only, "modifierProducer_mq");
    message_queue prodMod_mq(open_only, "producerModifier_mq");

    std::cout << "ile mess w prodMod? " << prodMod_mq.get_num_msg() << std::endl;
    std::cout << "ile mess w modProd? " << modProd_mq.get_num_msg() << std::endl;


    int messageBuffer[1];
    message_queue::size_type recvd_size;
    unsigned int priority;
    while( !modProd_mq.try_receive(&messageBuffer[0], sizeof(int), recvd_size, priority));

    std::cout << "nastepna wolna probka: " << messageBuffer[0] << std::endl;

    managed_shared_memory shMemory(open_only, "SoundBufferMemory");

    int *sample = shMemory.find<int>("producerModifierBuffer").first;

//    int pmbsize = shMemory.get_instance_length(sample);
//    std::cout << "pmbsize: " << pmbsize << std::endl;

    std::cout << "Place for sample created by supervisor: " << std::endl;
    displaySample(sample, BUFFSIZE);

    for (int j=0; j<BUFFSIZE; j++) {
        //put sample data into shared memory
        *(sample + messageBuffer[0] + j) = prodBuffer[j];
    }

    //send info about new data in buffer

    //TODO zawiesi sie jesli nie wysle wiadomosci - to raczej zle
    while (!prodMod_mq.try_send(&messageBuffer[0], sizeof (int), 0));
    std::cout << "ile mess w prodMod? " << prodMod_mq.get_num_msg() << std::endl;

    std::cout << "Producer. Data put into sample: " << std::endl;
    displaySample(sample, BUFFSIZE);

    std::cout << std::endl;


}

void displaySample(int *sample, int length){
    for (int j=0; j<length; j++){
        std::cout << *(sample+j) << ' ';
    }
    std::cout << '\n';
}