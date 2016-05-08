//
// Created by Lokotochek on 08.05.16.
//

#ifndef CPU_TASKS_TESTS_H
#define CPU_TASKS_TESTS_H

#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <string>
#include "Futex.h"
#include "FutexVolatile.h"

int x = 0, y = 1, n1 = 0, n2 = 0, iter = 1000;

volatile int protected_var = 0;
int simple_var = 0;

Futex ftx;
FutexVolatile ftx_volatile;

class Tests {

public:

    void startTest();
    void startVolatileTest();

};


#endif //CPU_TASKS_TESTS_H
