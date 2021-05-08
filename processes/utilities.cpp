//
// Created by laura on 06.05.2021.
//
#include <iostream>
#include "processes/utilities.h"

void displaySample(int *sample, int length){
    for (int j=0; j<length; j++){
        std::cout << *(sample+j) << ' ';
    }
    std::cout << '\n';
}

void displaySampleChar(char* buffer){

    for (int i = 0; i <BUFFSIZE; i++)
        std::cout << buffer[i];

    std::cout <<std::endl;
}