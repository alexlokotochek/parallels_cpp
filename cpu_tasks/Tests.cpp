//
// Created by Lokotochek on 08.05.16.
//

#include "Tests.h"


void workerVolatile(int &id, FutexVolatile &ftx, volatile int &protected_var) {
    while (iter > 0) {
        ftx.lock();
        if (iter == 0) {
            break;
        }
        if (id % 2 == 0) {
            if (x == 0 && y == 1) {
                ++n1;
                swap(x, y);
                iter--;
            }
        } else {
            if (x == 1 && y == 0) {
                ++n2;
                swap(x, y);
                iter--;
            }
        }

        protected_var++;

        ftx.unlock();
    }
}

void worker(int &id, Futex &ftx, int &protected_var) {
    while (iter > 0) {
        ftx.lock();
        if (iter == 0) {
            break;
        }
        if (id % 2 == 0) {
            if (x == 0 && y == 1) {
                ++n1;
                swap(x, y);
                iter--;
            }
        } else {
            if (x == 1 && y == 0) {
                ++n2;
                swap(x, y);
                iter--;
            }
        }

        protected_var++;

        ftx.unlock();
    }
}

void Tests::startVolatileTest() {

    vector<thread> threads_volatile;

    for (int i = 0; i < 10; ++i) {
        threads_volatile.push_back(thread (std::ref(workerVolatile), i, std::ref(ftx_volatile), std::ref(protected_var)));
    }

    for (auto &thr : threads_volatile) {
        thr.join();
    }

    cout << "woke up!" << endl;

    cout << n1 << " " << n2 << " ITER:" << iter << endl;
    cout << "volatile var should be equal to ITER : " << protected_var << endl;

}

void Tests::startTest() {
    cout << "start with simple variable" << endl;

    vector<thread> threads;


    for (int i = 0; i < 10; ++i) {
        threads.push_back(thread (worker,
                                  i,
                                  std::ref(ftx),
                                  std::ref(simple_var)));
    }

    for (auto &thr : threads) {
        thr.join();
    }

    cout << "woke up!" << endl;

    cout << n1 << " " << n2 << " ITER:" << iter << endl;
    cout << "simple var should be equal to ITER : " << simple_var << endl;

    cout << endl << "start with volatile variable" << endl;
}
