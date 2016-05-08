//
// Created by Lokotochek on 08.05.16.
//

#ifndef CPU_TASKS_FUTEX_H
#define CPU_TASKS_FUTEX_H

#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <string>

using namespace std;

class Futex {

    atomic<thread::id> ownerId;

    class futexException {
    public:

        futexException(const string &msg) : msg_(msg) {}

        string getMessage() const {
            return (msg_);
        }

    private:
        string msg_;
    };

public:

    Futex(): ownerId(thread::id()) {};

    Futex(const Futex &) = delete;

    void lock();
    bool try_lock();
    void unlock();

};


#endif //CPU_TASKS_FUTEX_H
