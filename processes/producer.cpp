//
// Created by laura on 30.04.2021.
//
#include "utilities.h"
#include "producer.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include <boost/interprocess/managed_shared_memory.hpp>

using namespace boost::interprocess;

void producer(){

    //TODO binding to CPU

    //TODO get samples from alsa into buffer
    int prodBuffer[BUFFSIZE];
    for (int i=0; i<BUFFSIZE; i++)
        prodBuffer[i]=i;

    //TODO: shared memory created by supervisor, producer -> open_only
    //shared_memory_object::remove("SoundBuffer");
    //std::cout << "jestem tu!";
    managed_shared_memory shMemory(open_only, "SoundBufferMemory");

    int *i = shMemory.find<int>("producerModifierBuffer").first;
    int *point = i;

//    int pmbsize = shMemory.get_instance_length(point);
//    std::cout << "pmbsize: " << pmbsize << std::endl;

    std::cout << "Place for sample created by supervisor: " << std::endl;
    displaySample(i, BUFFSIZE);

    for (int j=0; j<MEETING; j++) {
        //put sample data into shared memory
        *(point + j) = prodBuffer[j];
    }

//    *(point) = prodBuffer[0];
//    *(point+1) = prodBuffer[1];

    std::cout << "Producer. Data put into sample: " << std::endl;
    displaySample(i, BUFFSIZE);

    std::cout << std::endl;

//    std::pair<int*, std::size_t> p = managed_shm.find<int>("Sample");
//    if (p.first)
//    {
//        std::cout << *p.first << '\n';
//        std::cout << p.second << '\n';
//    }
//
//    int *found = managed_shm.find<int>("Sample").first;
//    std::cout << *found << '\n';
}

void displaySample(int *sample, int length){
    for (int j=0; j<length; j++){
        std::cout << *(sample+j) << ' ';
    }
    std::cout << '\n';
}