//
// Created by jakub on 01.05.2021.
//

#define SH_MEMORY_SIZE 65536
#define MQ_SIZE 200
#include "supervisor.h"
#include <boost/interprocess/containers/vector.hpp>


using namespace boost::interprocess;

typedef allocator<int, managed_shared_memory::segment_manager>  shMemAllocator;
typedef vector<int, shMemAllocator> sharedVector;


supervisor::supervisor() {
    init();
}

supervisor::~supervisor() {

    delete producerModifier_mq;
    delete modifierConsumer_mq;
    delete shMemory;

    message_queue::remove("producerModifier_mq");
    message_queue::remove("modifierConsumer_mq");
    shared_memory_object::remove("SoundBufferMemory");
}

void supervisor::init(){
    init_queues();
    init_shMemory();
    init_buffers();
}


void supervisor::init_shMemory(){

    shMemory = new managed_shared_memory (create_only, "SoundBufferMemory", SH_MEMORY_SIZE);
}

void supervisor::init_buffers() {
    shMemAllocator allocInstance(shMemory->get_segment_manager());

    shMemory->construct<sharedVector>("producerModifierBuffer")(allocInstance);
    shMemory->construct<sharedVector>("modifierConsumerBuffer")(allocInstance);
}

void supervisor::init_queues(){

    producerModifier_mq = new message_queue(create_only
            , "producerModifier_mq"
            , MQ_SIZE
            , sizeof(int)
    );

    modifierConsumer_mq = new message_queue(create_only
            , "modifierConsumer_mq"
            , MQ_SIZE
            , sizeof(int)
    );
}

