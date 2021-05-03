//
// Created by laura on 30.04.2021.
//
#include "utilities.h"
#include "producer.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include <boost/interprocess/managed_shared_memory.hpp>

#define PBUFFSIZE 10


using namespace boost::interprocess;

void producer(){

    //TODO binding to CPU

    //TODO get samples from alsa into buffer
    int prodBuffer[PBUFFSIZE];
    for (int i=0; i<PBUFFSIZE; i++)
        prodBuffer[i]=i;

    //TODO: shared memory created by supervisor, producer -> open_only
    shared_memory_object::remove("SoundBuffer");
    managed_shared_memory managed_shm{open_or_create, "SoundBuffer", 1024};

    int *i = managed_shm.construct<int>("Sample")[SAMPLESIZE](98);
    int *point = i;

    std::cout << "Producer. Place for sample created: " << std::endl;
    displaySample(point);

    for (int j=0; j<SAMPLESIZE; j++) {
        //put sample data into shared memory
        *(point + j) = prodBuffer[j];
    }

    std::cout << "Producer. Data put into sample: " << std::endl;
    displaySample(point);

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

void displaySample(int *sample){
    for (int j=0; j<SAMPLESIZE; j++){
        std::cout << *(sample+j) << ' ';
    }
    std::cout << '\n';
}