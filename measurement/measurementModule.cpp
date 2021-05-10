//
// Created by jakub on 10.05.2021.
//
#include <chrono>
#include "measurementModule.h"
#include <fstream>
#include <iostream>
#include <boost/interprocess/sync/named_semaphore.hpp>

using namespace boost::interprocess;
using namespace std::chrono;

measurementModule::measurementModule(int size, int mode) : size(size), sampleCounter(0) {



    if (mode == CREATE) {
        shared_memory_object::remove("memo");

        shMemory = new managed_shared_memory(create_only, "memo",  4096);

        shMemory->construct<int>("clock");
        shMemory->construct<time_point<system_clock>>("measurementBufferStart")[size];
        shMemory->construct<time_point<system_clock>>("measurementBufferEnd")[size];
    }else {

        shMemory = new managed_shared_memory (open_only, "memo");
    }


   int * tick = shMemory->find<int>("clock").first;

    bufferStart = shMemory->find<time_point<system_clock>>("measurementBufferStart").first;
    bufferEnd = shMemory->find<time_point<system_clock>>("measurementBufferEnd").first;

}

measurementModule::~measurementModule() {
    shared_memory_object::remove("measurementMemory");
}

void measurementModule::record(int index, int type) {
    if (type == START){
        bufferStart[index] = system_clock::now();

    }else if (type == END){
        bufferEnd[index] = system_clock::now();
    }
}


void measurementModule::saveToFile(char *outputFile) {
    std::ofstream outFile("data.txt");

    if (outFile.is_open()){

        for (int i = 0; i < size; i++){

            auto diff = bufferEnd[i] - bufferStart[i];

            std::time_t end = std::chrono::system_clock::to_time_t(bufferEnd[i]);
            std::time_t start = std::chrono::system_clock::to_time_t(bufferStart[i]);

        }

    }else{
        std::cout << "Unable to open a file!";
    }

    outFile.close();

}