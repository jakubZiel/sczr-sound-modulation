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


    alsa.openAlsa(PLAYBACK);

}

void consumer::writeToFile(char* file) {

    named_semaphore input(open_only, "userInputSem");

    input.wait();

    int loops = *(shMemory->find<int>("recordingTime").first);
    loops /= 725;

    latencyRecorder = new measurementModule(loops, OPEN);

    int maxLoops = loops;

    file_d = open(file, O_WRONLY);

    while (loops > 0) {

        receiveSamples();

        std::cout << "write loop:  " <<  loops << std::endl;

        writeSamples(maxLoops - loops);

        if (loops == 3)
            break;

        loops--;
    }

    close(file_d);

    latencyRecorder->saveToFile((char*) "data.txt");
}

void consumer::receiveSamples() {
    std::cout << "recv | mq mod-cons : " << modCons_mq->get_num_msg() << std::endl;

    modCons_mq->receive(&messageBuffer[0], sizeof(int), recvd_size, priority);


    sample = shMemory->find<char>("modifierConsumerBuffer").first;
    sample += messageBuffer[0] * BUFFSIZE;
}

void consumer::writeSamples(int currSample) {
    alsa.writeSample(sample, file_d);

    latencyRecorder->record(currSample, END);

    //get samples from memory into buffer
    std::cout << "send | mq cons-mod : " << consMod_mq->get_num_msg() << std::endl;

    consMod_mq->send(&messageBuffer[0], sizeof(int), 0);
}

consumer::~consumer() {

    delete modCons_mq;
    delete consMod_mq;
    delete shMemory;
    delete latencyRecorder;
}

int main(int argc, char *argv[])
{

    consumer Consumer;
    std::cout << "consumer\n\n";

    Consumer.writeToFile((char*) "out.wav");

    return 0;
}