//
// Created by laura on 06.05.2021.
//
#include <iostream>

void displaySample(int *sample, int length){
    for (int j=0; j<length; j++){
        std::cout << *(sample+j) << ' ';
    }
    std::cout << '\n';
}