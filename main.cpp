#include <iostream>
#include "supervisor/supervisor.h"
#include "sound/soundModule.h"
int main(int argc, char *argv[])
{
    cpu_set_t cpuSet;
    CPU_ZERO(&cpuSet);
    CPU_SET(7, &cpuSet);

    if (sched_setaffinity(0, sizeof(cpuSet), &cpuSet) == -1)
        std::cout << "didn't set the cpu :" << errno << std::endl;
    else
        std::cout << "cpu set to :" << sched_getcpu() << std::endl;


    char* mode = argv[1];

    supervisor SuperProc;

    bool start;

    std::cout << "Start recording?" << std::endl;

    std::cin >> start;

    std::cout << "How long do you want to record?" << std::endl;

    int howLong;

    std::cin >> howLong;

    std::cout << "How do you want to change sound volume" << std::endl;

    double volumeChange;

    std::cin >> volumeChange;

    if (mode != nullptr && strcmp(mode, "-verbose") == 0){

        system("gnome-terminal -e ./consumer");
        system("gnome-terminal -e ./producer");
        system("gnome-terminal -e ./modulator");

    }else {
        errno = -1;
        fprintf(stderr, "only verbose mode :%d\n", errno);
    }

    std::cout << "\n\n\n3 sec till recording" << std::endl;
    sleep(1);
    std::cout << "2 sec till recording" << std::endl;
    sleep(1);
    std::cout << "1 sec till recording" << std::endl;
    sleep(1);
    std::cout << "Recording started!" << std::endl;

    SuperProc.start(howLong, volumeChange);

    SuperProc.wait();

    std::cout << "Recording ended!" << std::endl;

    std::cout << "Do you want to play original back?" << std::endl << std::endl;

    bool playback;
    std::cin >> playback;

    howLong *= 1000000;

    if (playback){
        soundModule alsa;
        alsa.openAlsa(PLAYBACK);
        alsa.play(howLong, (char*)"original.wav");
    }

    std::cout << "Do you want to play x" << volumeChange << " version back?" << std::endl << std::endl;

    std::cin >> playback;

    if (playback){
        soundModule alsa;
        alsa.openAlsa(PLAYBACK);
        alsa.play(howLong, (char*)"out.wav");
    }
    return 0;
};
