//
// Created by jakub on 01.05.2021.
//

#ifndef SCZR_PROJEKT_SUPERVISOR_H
#define SCZR_PROJEKT_SUPERVISOR_H

#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/named_semaphore.hpp>
#include "measurement/measurementModule.h"
class supervisor {

    boost::interprocess::message_queue  *producerModifier_mq{};
    boost::interprocess::message_queue  *modifierProducer_mq{};
    boost::interprocess::message_queue  *modifierConsumer_mq{};
    boost::interprocess::message_queue  *consumerModifier_mq{};
    boost::interprocess::managed_shared_memory  *shMemory{};
    boost::interprocess::named_semaphore *startRecording{};
    boost::interprocess::named_semaphore *endRecording{};
    boost::interprocess::named_semaphore *userInputSem{};

    measurementModule* latencyRecorder{};


    void init_queues();
    void init_shMemory();
    void init_buffers();
    void init_sems();

    void init();
    static void removeAll();

public :
    void start(int howLong, double volumeChange);
    void wait();
    supervisor();
    ~supervisor();
};


#endif //SCZR_PROJEKT_SUPERVISOR_H
