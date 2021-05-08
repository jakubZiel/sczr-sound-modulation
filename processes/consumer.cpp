//
// Created by laura on 30.04.2021.
//

#include <iostream>
#include "utilities.h"

#include <boost/interprocess/managed_shared_memory.hpp>
#include <supervisor/supervisor.h>
#include <boost/interprocess/sync/named_semaphore.hpp>
#include "sound/soundModule.h"

using namespace boost::interprocess;



int main(int argc, char *argv[])
{

    //TODO binding to CPU

    //get access to queues
    message_queue modCons_mq(open_only, "modifierConsumer_mq");
    message_queue consMod_mq(open_only, "consumerModifier_mq");

    int messageBuffer[1];
    message_queue::size_type recvd_size;
    unsigned int priority;

    soundModule alsa;




    int file_d = open("sound.wav", O_WRONLY);
    int loops = 6897;


    while (loops > 0) {

        std::cout << "recv | mq mod-cons : " << modCons_mq.get_num_msg() << std::endl;

        modCons_mq.receive(&messageBuffer[0], sizeof(int), recvd_size, priority);

        //get access to shared memory
        managed_shared_memory shMemory(open_only, "SoundBufferMemory");
        char *sample = shMemory.find<char>("modifierConsumerBuffer").first;

        sample += messageBuffer[0] * BUFFSIZE;

        std::cout << "write sample" << 6897 - loops << std::endl;
        //alsa.writeSample(sample, file_d);
        loops--;

        //get samples from memory into buffer
        std::cout << "send | mq cons-mod : " << consMod_mq.get_num_msg() << std::endl;

        consMod_mq.send(&messageBuffer[0], sizeof(int), 0);
    }

    close(file_d);

    return 0;
}