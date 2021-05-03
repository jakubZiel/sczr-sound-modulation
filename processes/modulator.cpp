//
// Created by laura on 30.04.2021.
//

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "modulator.h"
#include "utilities.h"

#include <boost/interprocess/managed_shared_memory.hpp>

using namespace boost::interprocess;

void modulator(){

    //TODO binding to CPU

    //get access to sample in shared memory
    managed_shared_memory managed_shm{open_only, "SoundBuffer"};
    int *sample = managed_shm.find<int>("Sample").first;

    std::cout << "Modulator. Data received: " << std::endl;
    displaySample(sample);

    // change sample data
    for (int i=0; i<SAMPLESIZE; i++) {
        //put sample data into shared memory
        *(sample + i) = (*(sample+i))+10;
    }


    std::cout << "Modulator. Data changed: " << std::endl;
    displaySample(sample);
    std::cout << std::endl;

    //TODO signal the sample is done

}