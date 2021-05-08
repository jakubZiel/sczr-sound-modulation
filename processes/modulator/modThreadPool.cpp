//
// Created by jakub on 08.05.2021.
//

#include "modThreadPool.h"
#include <boost/interprocess/ipc/message_queue.hpp>
#include <processes/utilities.h>

using namespace boost::interprocess;
struct thread_arg{
    message_queue* consMod_mq;
    message_queue* prodMod;


    int loops;
};

void* modThreadPool::receiveConsMod(void *arg) {

    auto* queues = (thread_arg*) arg;

    auto* queue = queues->consMod_mq;
    int loops = queues->loops;

    int msgBuffer[0];
    message_queue::size_type recvd_size;
    unsigned int priority;

    while(loops > 0){

        queue->receive(msgBuffer, sizeof(int), recvd_size, priority);
        localConsMod_mq->send(msgBuffer, sizeof(int), priority);

        loops--;
    }

    return nullptr;
}

void* modThreadPool::receiveProdMod(void *arg) {
    auto* queues = (thread_arg*) arg;

    auto* queue = queues->prodMod;
    int loops = queues->loops;

    int msgBuffer[0];
    message_queue::size_type recvd_size;
    unsigned int priority;


    while(loops > 0){

        queue->receive(msgBuffer, sizeof(int), recvd_size, priority);
        localProdMod_mq->send(msgBuffer, sizeof(int), priority);

        loops--;
    }

    return nullptr;
}

modThreadPool::modThreadPool(){

    message_queue::remove("localProducerModifier_mq");
    message_queue::remove("localProducerModifier_mq");

    localProdMod_mq = new message_queue(create_only
            , "localProducerModifier_mq"
            , BUFFNUM
            , sizeof(int)
    );

    localProdMod_mq = new message_queue(create_only
            , "localModifierConsumer_mq"
            , BUFFNUM
            , sizeof(int)
    );



}


modThreadPool::~modThreadPool(){

}