#include "ProjectHeaders.h"
AlgorithmSelector::AlgorithmSelector() {
    count = 0;
    pos = 0;
    current = &rr;
    reason = "all servers have equal capacity";
}

void AlgorithmSelector::addTime(int t) {
    recent[pos] = t;
    pos = (pos + 1) % 10;
    if (count < 10)
        count++;
}

bool AlgorithmSelector::choose(vector<Server*>& servers) {
    Algorithm* newAlgo;

    // check if active servers have different capacities
    bool different = false;
    int firstCap = -1;
    for (int i = 0; i < (int)servers.size(); i++) {
        if (!servers[i]->isActive())
            continue;
        if (firstCap == -1)
            firstCap = servers[i]->getCapacity();
        else if (servers[i]->getCapacity() != firstCap)
            different = true;
    }
    int longest = 0, shortest = 1000;
    for (int i = 0; i < count; i++) {
        if (recent[i] > longest) longest = recent[i];
        if (recent[i] < shortest) shortest = recent[i];
    }

    if (count == 10 && longest > 3 * shortest) {
        newAlgo = &lc;
        reason = "request times vary from " + to_string(shortest) + " to " + to_string(longest) + " sec";
    } else if (different) {
        newAlgo = &wrr;
        reason = "servers have different capacities";
    } else {
        newAlgo = &rr;
        reason = "all servers have equal capacity";
    }

    if (newAlgo == current)
        return false;
    current = newAlgo;
    return true;
}

Algorithm* AlgorithmSelector::getCurrent() {
    return current;
}

string AlgorithmSelector::getReason() {
    return reason;
}
