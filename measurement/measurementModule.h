//
// Created by jakub on 10.05.2021.
//

#ifndef SCZR_SOUND_MOD_MEASUREMENTMODULE_H
#define SCZR_SOUND_MOD_MEASUREMENTMODULE_H
#include <boost/interprocess/managed_shared_memory.hpp>
#include <chrono>

#define START 0
#define END 1

#define CREATE 0
#define OPEN 1

class measurementModule {

    std::chrono::time_point<std::chrono::system_clock> startTime;

    int size;
    boost::interprocess::managed_shared_memory *shMemory;

    std::chrono::time_point<std::chrono::system_clock>* bufferStart;
    std::chrono::time_point<std::chrono::system_clock>* bufferEnd;

public:
    void record(int index, int type);
    explicit measurementModule(int size, int mode);
    ~measurementModule();

    void saveToFile(char* outputFile);
};


#endif //SCZR_SOUND_MOD_MEASUREMENTMODULE_H
