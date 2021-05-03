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
    managed_shared_memory shMemory(open_only, "SoundBufferMemory");

    int *i = shMemory.find<int>("producerModifierBuffer").first;
    int *sampleToModify = i;
//    int pmbsize = shMemory.get_instance_length("producerModifierBuffer");

//    if (sampleToModify)
//        std::cout << "jest cos w stm ";

    //get access to place for modified sample
    int *modifiedSample = shMemory.find<int>("modifierConsumerBuffer").first;
 //   int mcbsize = shMemory.get_instance_length("modifierConsumerBuffer");

    std::cout << "Modulator. Data received: " << std::endl;
    displaySample(sampleToModify, BUFFSIZE);

    // change sample data
    for (int i=0; i<MEETING; i++) {
        //put sample data into shared memory
        int toMod = *(sampleToModify + i);
        *(modifiedSample + i) = toMod*10;
        //*(sample + i) = (*(sample+i))+10;
    }


    std::cout << "Modulator. Data changed: " << std::endl;
    displaySample(modifiedSample, BUFFSIZE);
    std::cout << std::endl;

    //TODO signal the sample is done

}