#include "ProjectHeaders.h"
string LeastConnections::name() {
    return "Least Connections";
}

Server* LeastConnections::select(vector<Server*>& servers) {
    Server* best = NULL;
    for (int i = 0; i < (int)servers.size(); i++) {
        if (!servers[i]->isAvailable())
            continue;
        if (best == NULL || servers[i]->loadRatio() < best->loadRatio())
            best = servers[i];
    }
    if (best != NULL)
        reason = "least loaded (" + to_string(best->getLoad()) + "/" +
                 to_string(best->getCapacity()) + ")";
    return best;
}
