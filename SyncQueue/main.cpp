#include <iostream>
#include <thread>
#include <assert.h>
#include <vector>
#include <chrono>
#include <random>
#include <stack>
#include <utility>
#include <type_traits>
#include <cstddef>
#include <list>
#include <deque>
#include <queue>

using namespace std;
const int THR_NUM = 10;
const int ITERS = 1e4;

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> distr(5, 50);

namespace detection {

    template <class C>
    class operations{
    public:
        static void push_impl(const typename C::value_type &v, C &_data) {
            _data.push_back(v);
        }
        static typename C::value_type pop_impl(C &_data) {
            auto popped = _data.front();
            _data.pop_back();
            return popped;
        }
    };

    template <typename T>
    class operations<stack<T>>{
    public:
        static void push_impl(const T &v, stack<T> &_data) {
            _data.push(v);
        }
        static T pop_impl(stack<T> &_data) {
            auto popped = _data.top();
            _data.pop();
            return popped;
        }
    };

    template <typename T>
    class operations<queue<T>>{
    public:
        static void push_impl(const T &v, queue<T> &_data) {
            _data.push(v);
        }
        static T pop_impl(queue<T> &_data) {
            auto popped = _data.front();
            _data.pop();
            return popped;
        }
    };


}

template <class Array>
class SyncQueue {

    typedef typename Array::value_type value_type;
    Array data;
    mutex mtx;
    condition_variable queueNotEmpty;

public:

    void push(value_type &v){
        unique_lock<mutex> ul(mtx);
        detection::operations<Array> opclass;
        opclass.push_impl(v, data);
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
            detection::operations<Array> opclass;
            popped = opclass.pop_impl(data);
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
        detection::operations<Array> opclass;
        popped = opclass.pop_impl(data);
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


void thread_worker_vector(int myId, SyncQueue<vector<int>> &q) {
    int iters = 0;
    while (iters < ITERS) {
        printf("%d: %d\n", myId, iters);
        if (iters % 4 == 0){
            q.push(myId);
        }
        if (iters % 6 == 0){
            int popped = -1;
            // на ожидание стоит таймаут 500мс,
            // чтобы нормально протестировать
            bool res = q.popOrSleep(popped);
            if (res == true){
                // все элементы положительны, значит
                // если мы кого-то вытащили,
                // то он не может быть равен -1
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
                // все элементы положительны, значит
                // если мы кого-то вытащили,
                // то он не может быть равен -1
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

    cout << endl << endl << "success for vector";

    cout << endl << endl << "start stack" << endl << endl;

    testStack();

    cout << endl << endl << "success for stack";

    return 0;
}