#include "Tests.h"

int main() {

    CommonLine firstCommon, secondCommon;
    DiffLine firstDiff, secondDiff;
    Tests test;
    test.run<CommonLine, DiffLine>(firstCommon, secondCommon, firstDiff, secondDiff);

    return 0;
}