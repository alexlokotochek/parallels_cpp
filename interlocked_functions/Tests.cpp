#include <iostream>
#include "Tests.h"
using namespace chrono;


system_clock::time_point beginTimer;
double totalTime;

auto START = [] () {
    beginTimer = chrono::system_clock::now();
};

auto FINISH = [] () {
    totalTime = (chrono::system_clock::now() - beginTimer).count();
};

template<class C>
double Tests::store(C &first, C &second) {
    
    START();

    atomic_store(&first.a, 10000);
    atomic_store(&second.a, 10000);

    FINISH();
    
    return totalTime;
}
template<class C>
double Tests::load(C &first, C &second) {
    
    START();

    atomic_load(&first.a);
    atomic_load(&second.a);

    FINISH();
    
    return totalTime;
}
template<class C>
double Tests::add(C &first, C &second) {
    
    START();

    atomic_fetch_add(&first.a, 10000);
    atomic_fetch_add(&second.a, 10000);

    FINISH();
    
    return totalTime;
}

template<class C, class T>
void Tests::run(C &firstOne,
                C &firstTwo,
                T &secondOne,
                T &secondTwo) {
    
    cout << "Common vs Different cache lines." << endl << endl;

    atomic_init<int>(&firstOne.atomic, 0);
    atomic_init<int>(&secondOne.atomic, 0);
    atomic_init<int>(&firstTwo.atomic, 0);
    atomic_init<int>(&secondTwo.atomic, 0);

    cout << "load: " << endl;
    cout << load<C>(firstOne, firstTwo) << " vs ";
    cout << load<T>(secondOne, secondTwo) << endl << endl;

    cout << "store: " << endl;
    cout << store<C>(firstOne, firstTwo) << " vs ";
    cout << store<T>(secondOne, secondTwo) << endl << endl;

    cout << "add: " << endl;
    cout << add<C>(firstOne, firstTwo) << " vs ";
    cout << add<T>(secondOne, secondTwo) << endl << endl;
    
}