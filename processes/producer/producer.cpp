//
// Created by laura on 30.04.2021.
//
#include "processes/utilities.h"
#include "producer.h"

#include <iostream>
#include "sound/soundModule.h"
#include <boost/interprocess/sync/named_semaphore.hpp>

using namespace boost::interprocess;

producer::producer() {
    modProd_mq = new message_queue(open_only, "modifierProducer_mq");
    prodMod_mq = new message_queue(open_only, "producerModifier_mq");
    shMemory = new managed_shared_memory (open_only, "SoundBufferMemory");
}

producer::~producer() {
    delete modProd_mq;
    delete prodMod_mq;
    delete shMemory;
    delete latencyRecorder;
}

void producer::receiveAndSendSample(int currSample) {

    alsa.recordSample(prodBuffer);

    latencyRecorder->record(currSample, START);

    std::cout << "recv | mq mod-prod : " << modProd_mq->get_num_msg() << std::endl;

    modProd_mq->receive(&messageBuffer[0], sizeof(int), recvd_size, priority);

    sample = shMemory->find<char>("producerModifierBuffer").first;

    sample = sample + messageBuffer[0] * BUFFSIZE;

    for (int j = 0; j < BUFFSIZE; j++) {
        //put sample data into shared memory
        *(sample + j) = prodBuffer[j];
    }

    std::cout << "send | mq prod-mod : " << prodMod_mq->get_num_msg() << std::endl;

    prodMod_mq->send(&messageBuffer[0], sizeof(int), 0);
}

int producer::getAlsaVal() {
    return alsa.getVal();
}

void producer::recordAndProduce() {

    named_semaphore start(open_only, "startSemaphore");
    named_semaphore end(open_only, "endSemaphore");

    named_semaphore input(open_only, "userInputSem");

    input.wait();


    int loops = *(shMemory->find<int>("recordingTime").first);
    loops /= 725;

    latencyRecorder = new measurementModule(loops, OPEN);

    start.wait();

    alsa.openAlsa(RECORD);

    int maxLoops = loops;

    while (loops > 0) {

        receiveAndSendSample(maxLoops - loops);

        std::cout << loops << std::endl;

        loops--;
    }

    end.post();

}

int main(int argc, char *argv[])
{
    //testing purposes
    /*
    cpu_set_t cpuSet;
    CPU_ZERO(&cpuSet);
    CPU_SET(7, &cpuSet);
    CPU_SET(6, &cpuSet);
    CPU_SET(5, &cpuSet);
    CPU_SET(4, &cpuSet);
    if (sched_setaffinity(0, sizeof(cpuSet), &cpuSet) == -1)
        std::cout << "didn't set the cpu :" << errno << std::endl;
    else
        std::cout << "cpu set to :" << sched_getcpu() << std::endl;
    */
    producer Producer;
    std::cout << "producer\n\n";

    Producer.recordAndProduce();

    return 0;
}
