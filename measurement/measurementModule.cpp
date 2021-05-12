//
// Created by jakub on 10.05.2021.
//
#include <chrono>
#include "measurementModule.h"
#include <fstream>
#include <iostream>
#include <boost/interprocess/sync/named_semaphore.hpp>
#include "processes/utilities.h"

using namespace boost::interprocess;
using namespace std::chrono;

measurementModule::measurementModule(int size, int mode) : size(size){

    if (mode == CREATE) {
        shared_memory_object::remove("memo");

        shMemory = new managed_shared_memory(create_only, "memo",  100 * 4096);

        shMemory->construct<time_point<system_clock>>("measurementBufferStart")[size]();
        shMemory->construct<time_point<system_clock>>("measurementBufferEnd")[size]();


    }else {

        shMemory = new managed_shared_memory (open_only, "memo");
    }

    bufferStart = shMemory->find<time_point<system_clock>>("measurementBufferStart").first;
    bufferEnd = shMemory->find<time_point<system_clock>>("measurementBufferEnd").first;
}

measurementModule::~measurementModule() {
    shared_memory_object::remove("memo");
}

void measurementModule::record(int index, int type) {

    std::cout << size << std::endl;

    if (type == START){
        bufferStart[index] = system_clock::now();

    }else if (type == END){
        bufferEnd[index] = system_clock::now();

    }
}


void measurementModule::saveToFile(char *outputFile) {
    std::ofstream outFile("data.txt");

    int frequency = size / DISPLAYEDSAMPLES;

    if (outFile.is_open()){

        double sum = 0;

        for (int i = 0; i < size; i++){

            auto diff = bufferEnd[i] - bufferStart[i];

            auto diff2 = duration_cast<microseconds>(bufferEnd[i] - bufferStart[i]);

            sum += diff.count();

            if (i % frequency == 0){
                outFile << sum / frequency / 1000 << std::endl;
                sum = 0;
            }
        }

        time_t now = time(0);
        char* dt = ctime(&now);

        outFile << std::endl << "measured: " << dt;

    }else{
        std::cout << "Unable to open a file!";
    }

    outFile.close();

}