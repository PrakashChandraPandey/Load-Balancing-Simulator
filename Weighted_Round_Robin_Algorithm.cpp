#include "ProjectHeaders.h"
WeightedRoundRobin::WeightedRoundRobin() {
    current = 0;
    given = 0;
}

string WeightedRoundRobin::name() {
    return "Weighted Round Robin";
}

Server* WeightedRoundRobin::select(vector<Server*>& servers) {
    int n = servers.size();
    if (n == 0)
        return NULL;

    for (int i = 0; i <= n; i++) {
        if (current >= n)
            current = 0;
        Server* s = servers[current];
        if (s->isAvailable() && given < s->getCapacity()) {
            given++;
            reason = "weight " + to_string(s->getCapacity()) + ", request " +
                     to_string(given) + " of its turn";
            return s;
        }
        current = (current + 1) % n;   
        given = 0;
    }
    return NULL;
}
