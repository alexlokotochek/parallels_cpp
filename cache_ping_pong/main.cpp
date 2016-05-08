#include <atomic>
#include <cstdio>
#include <vector>
#include <chrono>
#include <thread>
#include <iostream>
#include <assert.h>
#include <stddef.h>
#include <sys/sysctl.h>

using namespace std;

size_t cache_line_size() {
    size_t line_size = 0;
    size_t sizeof_line_size = sizeof(line_size);
    sysctlbyname("hw.cachelinesize", &line_size, &sizeof_line_size, 0, 0);
    return line_size;
}

struct MyStruct {
    int a;
    int b;
    MyStruct() : a(0), b(0) {};
};

struct MyShiftStruct {
    int a;
    int shift[12345];
    int b;
    MyShiftStruct() : a(0), b(0) {};
};

MyStruct mystruct;
MyShiftStruct myshiftstruct;

auto worker = [](int &number) {
    for (int i = 0; i < 1e9; ++i) {
        number = (1289732*number+19238);
    }
};

void testPingPong() {
    chrono::time_point<chrono::system_clock> start_timer, end_timer;
    start_timer = chrono::system_clock::now();

    thread first = thread(worker, ref(mystruct.a));
    thread second = thread(worker, ref(mystruct.b));

    first.join();
    second.join();

    end_timer = chrono::system_clock::now();
    chrono::duration<double> elapsed_seconds = end_timer - start_timer;

    assert(mystruct.a == mystruct.b);
    cout << "ping-pong: " << elapsed_seconds.count() << " seconds" << endl;
}

void testNoPingPong() {
    chrono::time_point<chrono::system_clock> start_timer, end_timer;
    start_timer = chrono::system_clock::now();

    thread first = thread(worker, ref(myshiftstruct.a));
    thread second = thread(worker, ref(myshiftstruct.b));

    first.join();
    second.join();

    end_timer = chrono::system_clock::now();
    chrono::duration<double> elapsed_seconds = end_timer - start_timer;

    assert(myshiftstruct.a == myshiftstruct.b);
    cout << "without ping-pong: " << elapsed_seconds.count() << " seconds" << endl;
}

int main() {
    cout << "Cache line size: " << cache_line_size() << endl;
    testNoPingPong();
    testPingPong();

    return 0;
}