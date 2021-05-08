//
// Created by jakub on 01.05.2021.
//

#define SH_MEMORY_SIZE 65536
#define MQ_SIZE 200
#include "supervisor.h"
#include "processes/utilities.h"
#include <iostream>



using namespace boost::interprocess;

supervisor::supervisor() {
    std::cout << "in constructor" << std::endl;

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

    removeAll();
}

void supervisor::removeAll() {
    message_queue::remove("producerModifier_mq");
    message_queue::remove("modifierProducer_mq");
    message_queue::remove("modifierConsumer_mq");
    message_queue::remove("consumerModifier_mq");
    shared_memory_object::remove("SoundBufferMemory");
}

void supervisor::init(){
    init_queues();
    std::cout << "queues" << std::endl;


    init_shMemory();
    std::cout << "memory" << std::endl;


    init_buffers();
    std::cout << "buffers" << std::endl;

}

void supervisor::init_shMemory(){

    shMemory = new managed_shared_memory (open_or_create, "SoundBufferMemory", SH_MEMORY_SIZE);

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



