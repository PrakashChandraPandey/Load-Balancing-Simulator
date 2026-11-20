#include "ProjectHeaders.h"
RoundRobin::RoundRobin() {
    next = 0;
}

string RoundRobin::name() {
    return "Round Robin";
}

Server* RoundRobin::select(vector<Server*>& servers) {
    int n = servers.size();
    for (int i = 0; i < n; i++) {
        int k = (next + i) % n;
        if (servers[k]->isAvailable()) {
            next = (k + 1) % n;
            reason = "next in turn";
            return servers[k];
        }
    }
    return NULL;   // no server is free
}
