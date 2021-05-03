//
// Created by laura on 30.04.2021.
//

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "consumer.h"
#include "utilities.h"

#include <boost/interprocess/managed_shared_memory.hpp>

#define CBUFFSIZE 10

using namespace boost::interprocess;

void consumer(){

    //TODO binding to CPU

    //get access to shared memory
    managed_shared_memory managed_shm{open_only, "SoundBuffer"};
    int *sample = managed_shm.find<int>("Sample").first;

    std::cout << "Consumer. Data received: " << std::endl;
    displaySample(sample);

    //get samples from memory into buffer
    int consBuffer[CBUFFSIZE];

    std::cout << "Consumer. Data from sample put into buffer: " << std::endl;
    for (int j=0; j<SAMPLESIZE; j++) {
        consBuffer[j] = *(sample + j);
        std::cout << consBuffer[j] << " ";
    }

    std::cout << std::endl;

    //TODO put changed samples to alsa buffer



    //TODO: GET RID OF THE SAMPLE?
    std::cout << "Consumer. Destroying sample: " << std::endl;
    managed_shm.destroy<int>("Sample");
    std::pair<int*, std::size_t> p = managed_shm.find<int>("Sample");
    std::cout << p.first << '\n';

}