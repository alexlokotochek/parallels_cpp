//
// Created by Lokotochek on 08.05.16.
//

#ifndef CPU_TASKS_FUTEXVOLATILE_H
#define CPU_TASKS_FUTEXVOLATILE_H

#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <string>

using namespace std;

class FutexVolatile {

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

    FutexVolatile(): ownerId(thread::id()) {};

    FutexVolatile(const FutexVolatile &) = delete;

    void lock();
    bool try_lock();
    void unlock();

};


#endif //CPU_TASKS_FUTEXVOLATILE_H
