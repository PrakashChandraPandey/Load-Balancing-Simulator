// Server failure module
#include "ProjectHeaders.h"
// request from a failed server goes back to the queue
void LoadBalancer::moveBack(Request r) {
    r.moves++;
    if (r.moves > 3) {
        failedRequests++;
        log->write(time, "REQUEST FAILED", info(r) + " - moved more than 3 times");
        return;
    }
    if (!waiting.enqueue(r)) {
        rejected++;
        log->write(time, "REJECTED", info(r) + " - queue is full");
    } else {
        log->write(time, "MOVED", info(r) + " -> back to the waiting queue");
    }
}

bool LoadBalancer::failServer(int id) {
    Server* s = findServer(id);
    if (s == NULL || !s->isActive())
        return false;

    vector<Request> pending = s->fail();
    serverFailures++;
    log->write(time, "FAILED", "Server " + to_string(id) + " failed | " +
               to_string(pending.size()) + " requests will be moved");
    for (int i = 0; i < (int)pending.size(); i++)
        moveBack(pending[i]);
    assignRequests();
    return true;
}
