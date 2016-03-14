#include <iostream>
#include <thread>
#include <assert.h>
#include <vector>
#include <chrono>
#include <atomic>
#include <string>
#include <mutex>
using namespace std;

class Futex {
    
    atomic<int> owner;
    
public:
    
    Futex(const Futex &) = delete;
    Futex() = default;
    
    void initOwner() {
        owner.store(-1);
    }

    void lock(int threadId) {
        int expected = -1;
        while (!owner.compare_exchange_strong(expected, threadId)) {
            expected = -1;
        }
    }
    
    void unlock(int threadId) {
        int freeFlag = -1, toReplace = threadId;;
        if (!owner.compare_exchange_strong(threadId, freeFlag)) {
            cerr << "Current owner " << threadId << " but "
            << "Thread number " + to_string(toReplace)
            << " is not the owner!" << endl;
        }
    }
    
};

void joinAll(vector<thread> &threads){
    for (auto &t: threads) {
        t.join();
    }
}

void worker(int threadId,
            int targ_max,
            int &target,
            vector<int> &incrementsCounter,
            Futex &futex) {
    while(true) {
        futex.lock(threadId);
        if (target < targ_max) {
            target++;
            incrementsCounter[threadId]++;
            futex.unlock(threadId);
        } else {
            futex.unlock(threadId);
            break;
        }
    }
}

void run(int thr_num=10, int targ_max=2*(1e7), bool print=true) {
    int target = 0;
    vector<int> incrementsCounter(thr_num, 0);
    vector<thread> threads;
    Futex futex;
    futex.initOwner();
    
    for (int i = 0; i < thr_num; ++i) {
        threads.push_back(thread(worker,
                                 i,
                                 targ_max,
                                 ref(target),
                                 ref(incrementsCounter),
                                 ref(futex)));
    }
    
    joinAll(threads);
    
    int incrementsSum = 0;
    
    for (int i = 0; i < thr_num; ++i) {
        if (print) {
            cout << "thread " << i << ": "
            << incrementsCounter[i] << " increments" << endl;
        }
        incrementsSum += incrementsCounter[i];
    }
    
    assert(target == incrementsSum && target == targ_max);
    
    if (print) {
        cout << "target: " << target << endl;
        cout << "incsum: " << incrementsSum << endl;
        cout << "wanted: " << targ_max << endl
        << "-------------------------------" << endl;
    }
}

void test(int thr_num, int targ_max) {
    auto start = std::chrono::system_clock::now();
    run(thr_num, targ_max, false);
    auto end = std::chrono::system_clock::now();
    cout << "time for " << thr_num << " threads and target < " << targ_max << ": ";
    cout << chrono::duration<double>(end - start).count() << endl
    << "-------------------------------" << endl;
}

void mutexWorker(int threadId,
                 int targ_max,
                 int &target,
                 vector<int> &incrementsCounter,
                 mutex &mtx) {
    while(true) {
        
        mtx.lock();
        //while (!mtx.try_lock()){};
        
        if (target < targ_max) {
            target++;
            incrementsCounter[threadId]++;
            mtx.unlock();
        } else {
            mtx.unlock();
            break;
        }
    }
}

void testMutex(int thr_num, int targ_max) {
    auto start = std::chrono::system_clock::now();
    int target = 0;
    vector<int> incrementsCounter(thr_num, 0);
    vector<thread> threads;
    mutex mtx;
    
    for (int i = 0; i < thr_num; ++i) {
        threads.push_back(thread(mutexWorker,
                                 i,
                                 targ_max,
                                 ref(target),
                                 ref(incrementsCounter),
                                 ref(mtx)));
    }
    
    joinAll(threads);
    
    int incrementsSum = 0;
    
    for (int i = 0; i < thr_num; ++i) {
        incrementsSum += incrementsCounter[i];
    }

    assert(target == incrementsSum && target == targ_max);
    
    auto end = std::chrono::system_clock::now();
    cout << "std::mutex time for " << thr_num << " threads and target < " << targ_max << ": ";
    cout << chrono::duration<double>(end - start).count() << endl
    << "-------------------------------" << endl;
}

int main() {

    // пункт В --------------------------------------------------------------
    
//    run();
    
    // пункт С --------------------------------------------------------------
    
    int thr_num = thread::hardware_concurrency();
    int targ_max = 1*(1e7);
    testMutex(thr_num, targ_max);
    test(thr_num, targ_max);

    thr_num = thread::hardware_concurrency()/2;
    testMutex(thr_num, targ_max);
    test(thr_num, targ_max);
    
    thr_num = thread::hardware_concurrency()*2;
    testMutex(thr_num, targ_max);
    test(thr_num, targ_max);
    
    return 0;
}
