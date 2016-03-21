#include <iostream>
#include <thread>
#include <assert.h>
#include <vector>
#include <chrono>
#include <random>
#include <stack>
#include <cstddef>
#include <queue>

using namespace std;
const int THR_NUM = 10;
const int ITERS = 1e4;

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> distr(5, 50);

template <typename T>
class detect {
public:

    template<class C>
    static void push_impl(T &v);

    template<class C>
    static T pop_impl();

    template<stack<T> C>
    static void push_impl<C>(T &v) {}

    template<queue<T> C>
    static void push_impl<C>(T &v) {}

    template<stack<T> C>
    static T pop_impl<C>() {}

    template<queue<T> C>
    static T pop_impl<C>() {}
};

//push_back() и pop_back(): vector<>, deque<>, list<>
template <class Array>
class SyncQueue {
    typedef typename Array::value_type value_type;
    Array data;
    mutex mtx;
    condition_variable queueNotEmpty;

    void push_impl(value_type &v) {
        detect::push_impl(v);
    }

    value_type pop_impl() {
        detect::pop_impl();
    }
//    template <class C>
//    void push_impl(value_type &v);
//
//
//    template <class C>
//    value_type pop_impl();
//
//    template<>
//    void push_impl<stack<value_type>>(value_type &v) {
//        data.push(v);
//    };
//
//    template<>
//    void push_impl<queue<value_type>>(value_type &v) {
//        data.push(v);
//    };
//
//    template<>
//    value_type pop_impl<stack<value_type>>() {
//        auto popped = data.front();
//        data.pop_back();
//        return popped;
//    }
//
//    template<>
//    value_type pop_impl<queue<value_type>>() {
//        auto popped = data.front();
//        data.pop_back();
//        return popped;
//    }

public:

    void push(value_type &v){
        unique_lock<mutex> ul(mtx);
        push_impl(v);
        ul.unlock();
        queueNotEmpty.notify_one();
    };

    bool popOrSleep(value_type &popped){
        unique_lock<mutex> ul(mtx);
        auto handler = [this]{return !this->data.empty();};
        if (queueNotEmpty.wait_for(ul,
                                   std::chrono::milliseconds(500),
                                   handler) == true)
        {
            popped = pop_impl();
            ul.unlock();
            return true;
        } else {
            ul.unlock();
            return false;
        }
    }

    bool popNoSleep(value_type &popped){
        unique_lock<mutex> ul(mtx);
        if (data.empty()){
            ul.unlock();
            return false;
        }
        popped = pop_impl();
        ul.unlock();
        return true;
    }

    int size() {
        unique_lock<mutex> ul(mtx);
        auto size = data.size();
        ul.unlock();
        return size;
    }

    bool empty() {
        return (this->size() == 0);
    }
};

//template<class C>
//void SyncQueue::push_impl(value_type &v) {
//    data.push_back(v);
//}
//
//template<class C>
//value_type SyncQueue::pop_impl() {
//    return data.pop();
//};





void thread_worker_vector(int myId, SyncQueue<vector<int>> &q) {
    int iters = 0;
    while (iters < ITERS) {
        printf("%d: %d\n", myId, iters);
        if (iters % 4 == 0){
            q.push(myId);
        }
        if (iters % 6 == 0){
            int popped = -1;
            bool res = q.popOrSleep(popped);
            if (res == true){
                assert(popped != -1);
                printf("successfully sleepPopped %d\n", popped);
            } else {
                printf("didn't sleepPop because of timeout\n");
            }
        }
        if (iters % 6 == 1){
            int popped = -1;
            bool res = q.popNoSleep(popped);
            if (res == true) {
                assert(popped != -1);
                printf("successfully noSleepPopped %d\n", popped);
            } else {
                printf("unsuccessful noSleepPopping\n");
            }
        }
        iters += distr(gen);
    }
}

void thread_worker_stack(int myId, SyncQueue<stack<int>> &q) {
    int iters = 0;
    while (iters < ITERS) {
        printf("%d: %d\n", myId, iters);
        if (iters % 4 == 0){
            q.push(myId);
        }
        if (iters % 6 == 0){
            int popped = -1;
            bool res = q.popOrSleep(popped);
            if (res == true){
                assert(popped != -1);
                printf("successfully sleepPopped %d\n", popped);
            } else {
                printf("didn't sleepPop because of timeout\n");
            }
        }
        if (iters % 6 == 1){
            int popped = -1;
            bool res = q.popNoSleep(popped);
            if (res == true) {
                assert(popped != -1);
                printf("successfully noSleepPopped %d\n", popped);
            } else {
                printf("unsuccessful noSleepPopping\n");
            }
        }
        iters += distr(gen);
    }
}

void testVector() {
    SyncQueue<vector<int>> q;

    vector<thread> threads;

    for (int thr_id = 0; thr_id < THR_NUM; ++thr_id) {
        threads.push_back(thread(thread_worker_vector,
                                 thr_id,
                                 std::ref(q))
        );
    }

    for (int i = 0; i < THR_NUM; ++i){
        threads[i].join();
    }
}

void testStack() {
    SyncQueue<stack<int>> q;

    vector<thread> threads;

    for (int thr_id = 0; thr_id < THR_NUM; ++thr_id) {
        threads.push_back(thread(thread_worker_stack,
                                 thr_id,
                                 std::ref(q))
        );
    }

    for (int i = 0; i < THR_NUM; ++i){
        threads[i].join();
    }
}


int main(){
    cout << "start vector" << endl << endl;

    testVector();

    cout << endl << endl << "success vector";

    cout << endl << endl << "start stack" << endl << endl;

    testStack();

    cout << endl << endl << "success stack";

    return 0;
}