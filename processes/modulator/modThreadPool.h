//
// Created by jakub on 08.05.2021.
//

#ifndef SCZR_SOUND_MOD_MODTHREADPOOL_H
#define SCZR_SOUND_MOD_MODTHREADPOOL_H
#include <boost/interprocess/ipc/message_queue.hpp>
#include <pthread.h>

class modThreadPool {

    pthread_t  threadProdMod_mq;
    pthread_t  threadConsProd_mq;


    boost::interprocess::message_queue *localConsMod_mq;
    boost::interprocess::message_queue *localProdMod_mq;


public:

    modThreadPool();
    ~modThreadPool();
    void* receiveConsMod(void* arg);
    void* receiveProdMod(void* arg);


};


#endif //SCZR_SOUND_MOD_MODTHREADPOOL_H
