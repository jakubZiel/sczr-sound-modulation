#include <iostream>
#include "supervisor/supervisor.h"
#include "sound/soundModule.h"
int main(int argc, char *argv[])
{

    char* mode = argv[1];

    supervisor SuperProc;

    bool start;

    std::cout << "Start recording?" << std::endl;

    std::cin >> start;


    std::cout << "How long do you want to record?" << std::endl;

    int howLong;

    std::cin >> howLong;


    if (mode != nullptr && strcmp(mode, "-verbose") == 0){

        system("gnome-terminal -e ./consumer");
        system("gnome-terminal -e ./producer");
        system("gnome-terminal -e ./modulator");

    }else {
        errno = -1;
        fprintf(stderr, "only verbose mode :%d\n", errno);
        return -1;
    }



    std::cout << "\n\n\n3 sec till recording" << std::endl;
    sleep(1);
    std::cout << "2 sec till recording" << std::endl;
    sleep(1);
    std::cout << "1 sec till recording" << std::endl;
    sleep(1);
    std::cout << "Recording started!" << std::endl;

    SuperProc.start(howLong);

    SuperProc.wait();

    std::cout << "Recording ended!" << std::endl;

    std::cout << "Do you want to play it back?" << std::endl << std::endl;

    bool playback;
    std::cin >> playback;

    if (playback){
        soundModule alsa;
        alsa.openAlsa(PLAYBACK);
        alsa.play(5000000, (char*)"out.wav");
    }

    return 0;
};