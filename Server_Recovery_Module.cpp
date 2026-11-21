#include "ProjectHeaders.h"
bool LoadBalancer::recoverServer(int id) {
    Server* s = findServer(id);
    if (s == NULL || s->isActive())
        return false;

    s->recover();
    log->write(time, "RECOVERED", "Server " + to_string(id) + " is active again");
    assignRequests();
    return true;
}
