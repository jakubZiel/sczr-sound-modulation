//
// Created by jakub on 10.05.2021.
//

#include "measurementModule.h"
#include <iostream>
int main(){

    measurementModule measure(10, CREATE);
    int x;
    for (int i = 0; i < 10; i++){

        std::cout << "start\n";
        std::cin >> x;

        measure.record(i, START);

        std::cout << "end\n";
        std::cin >> x;

        measure.record(i, END);
    }

    measure.saveToFile((char*) "data.txt");

    return 0;
}