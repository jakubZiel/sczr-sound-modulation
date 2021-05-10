//
// Created by jakub on 01.05.2021.
//

#define SH_MEMORY_SIZE 65536
#define MQ_SIZE 200
#include "supervisor.h"
#include "processes/utilities.h"
#include <iostream>
#include <boost/interprocess/managed_shared_memory.hpp>
#include "measurement/measurementModule.h"

using namespace boost::interprocess;

supervisor::supervisor() {


    removeAll();

    init();
}

supervisor::~supervisor() {

    std::cout << "in destructor" << std::endl;


    delete producerModifier_mq;
    delete modifierProducer_mq;
    delete modifierConsumer_mq;
    delete consumerModifier_mq;
    delete shMemory;
    delete startRecording;
    delete endRecording;
    delete userInputSem;

    removeAll();
}

void supervisor::removeAll() {
    message_queue::remove("producerModifier_mq");
    message_queue::remove("modifierProducer_mq");
    message_queue::remove("modifierConsumer_mq");
    message_queue::remove("consumerModifier_mq");
    named_semaphore::remove("startSemaphore");
    named_semaphore::remove("endSemaphore");
    shared_memory_object::remove("SoundBufferMemory");
    named_semaphore::remove("userInputSem");

}

void supervisor::init(){
    init_queues();

    init_shMemory();

    init_buffers();

    init_sems();
    std::cout << "supervisor initialized" << std::endl;
}

void supervisor::start(int howLong){

    int howLongMicroS = howLong * 1000000;

    shMemory->construct<int>("recordingTime")(howLongMicroS);


    //TODO
    latencyRecorder = new measurementModule(howLong / 725, CREATE);


    for (int i = 0; i < 3; i++)
        userInputSem->post();

    startRecording->post();
}

void supervisor::wait() {
    endRecording->wait();
}

void supervisor::init_shMemory(){

    shMemory = new managed_shared_memory (open_or_create, "SoundBufferMemory", SH_MEMORY_SIZE / 16);

}

void supervisor::init_buffers() {

    shMemory->construct<char>("producerModifierBuffer")[BUFFSIZE*BUFFNUM](0);
    shMemory->construct<char>("modifierConsumerBuffer")[BUFFSIZE*BUFFNUM](0);
}

void supervisor::init_queues(){

    //indexes of read samples
    producerModifier_mq = new message_queue(create_only
            , "producerModifier_mq"
            , BUFFNUM
            , sizeof(int)
    );

    //where can the read be put
    modifierProducer_mq = new message_queue(create_only
            , "modifierProducer_mq"
            , BUFFNUM
            , sizeof(int)
    );

    //fill modifierProducer_mq with initial values
    int bufor[1];
    for(int i=0; i<BUFFNUM; i++){
        bufor[0]=i;
        modifierProducer_mq->send(&bufor, sizeof (int), 0);
    }

    //indexes of modified samples
    modifierConsumer_mq = new message_queue(create_only
            , "modifierConsumer_mq"
            , MQ_SIZE
            , sizeof(int)
    );

    //where can modified be put
    consumerModifier_mq = new message_queue(create_only
            , "consumerModifier_mq"
            , MQ_SIZE
            , sizeof(int)
    );

    //fill consumerModifier_mq with initial values
    for(int i=0; i<BUFFNUM; i++){
        bufor[0]=i;
        consumerModifier_mq->send(&bufor, sizeof (int), 0);
    }
}

void supervisor::init_sems(){
    startRecording = new named_semaphore(create_only, "startSemaphore", 0);
    endRecording = new named_semaphore(create_only, "endSemaphore", 0);
    userInputSem = new named_semaphore(create_only, "userInputSem", 0);
}

