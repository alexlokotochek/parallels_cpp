#include <chrono>
#include <atomic>

using namespace std;

// без выравнивания
struct CommonLine {
    atomic<int> a;
};

// с выравниванием
struct DiffLine {
    atomic<int> a;
    int b[12345];
};

class Tests {

private:

    template<class C>
    double store(C &first, C &second);

    template<class C>
    double load(C &first, C &second);

    template<class C>
    double add(C &first, C &second);

public:

    template<class C, class T>
    void run(C &firstOne,
             C &firstTwo,
             T &secondOne,
             T &secondTwo);
};