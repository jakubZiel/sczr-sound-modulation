//
// Created by laura on 30.04.2021.
//

#include <iostream>
#include "processes/utilities.h"

#include <supervisor/supervisor.h>
#include "consumer.h"
#include "sound/soundModule.h"



using namespace boost::interprocess;

consumer::consumer(){

    modCons_mq = new message_queue(open_only, "modifierConsumer_mq");
    consMod_mq = new message_queue(open_only, "consumerModifier_mq");
    shMemory = new managed_shared_memory(open_only, "SoundBufferMemory");

    //consumer doesn't playback, but it has to be initialized
    alsa.openAlsa(PLAYBACK);
}

void consumer::writeToFile(char* file) {

    named_semaphore input(open_only, "userInputSem");

    input.wait();

    int loops = *(shMemory->find<int>("recordingTime").first);
    loops /= 725;

    latencyRecorder = new measurementModule(loops, OPEN);

    int maxLoops = loops;

    truncate(file, 0);
    file_d = open(file, O_WRONLY);

    char *original_file = (char*) "original.wav";

    truncate(original_file, 0);
    original_file_d = open(original_file, O_WRONLY);


    while (loops > 0) {

        receiveSamples();

        std::cout << "write loop:  " <<  loops << std::endl;

        writeSamples(maxLoops - loops);

        if (loops == 3)
            break;

        loops--;
    }

    close(file_d);
    close(original_file_d);

    latencyRecorder->saveToFile((char*) "data.txt");
}

void consumer::receiveSamples() {
    std::cout << "recv | mq mod-cons : " << modCons_mq->get_num_msg() << std::endl;

    modCons_mq->receive(&messageBuffer[0], sizeof(int), recvd_size, priority);

    sample = shMemory->find<char>("modifierConsumerBuffer").first;
    sample += messageBuffer[0] * BUFFSIZE;

    originalSample = shMemory->find<char>("unmodifiedBuffer").first;
    originalSample += messageBuffer[0] * BUFFSIZE;
}
void consumer::writeSamples(int currSample) {
    alsa.writeSample(sample, file_d);

    latencyRecorder->record(currSample, END);
    alsa.writeSample(originalSample, original_file_d);



    //get samples from memory into buffer
    std::cout << "send | mq cons-mod : " << consMod_mq->get_num_msg() << std::endl;

    consMod_mq->send(&messageBuffer[0], sizeof(int), 0);
}

consumer::~consumer() {

    delete modCons_mq;
    delete consMod_mq;
    delete shMemory;
    delete latencyRecorder;
    delete originalSample;
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
    consumer Consumer;
    std::cout << " consumer\n\n";

    Consumer.writeToFile((char*) "out.wav");

    return 0;
}
