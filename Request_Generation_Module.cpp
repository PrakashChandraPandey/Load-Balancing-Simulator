#include "ProjectHeaders.h"
#include <cstdlib>

const char* paths[6] = {"/login", "/browse", "/search", "/upload", "/report", "/video"};
const int minTime[6] = {1, 3, 11, 15, 21, 25};
const int maxTime[6] = {3, 5, 15, 20, 25, 30};

RequestGenerator::RequestGenerator() {
    nextId = 1;
    testMode = false;
}

void RequestGenerator::setTestMode(bool mode) {
    testMode = mode;
}

int RequestGenerator::randomBetween(int low, int high) {
    return low + rand() % (high - low + 1);
}

Request RequestGenerator::next(int time) {
    int k;
    if (testMode) {
        k = 1;                          // /browse
    } else {
        int r = rand() % 100;
        if (r < 70)
            k = randomBetween(0, 1);    // short
        else if (r < 90)
            k = randomBetween(2, 3);    // medium
        else
            k = randomBetween(4, 5);    // long
    }
    Request req = makeRequest(nextId, paths[k], randomBetween(minTime[k], maxTime[k]), time);
    nextId++;
    return req;
}
