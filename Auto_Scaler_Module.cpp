#include "ProjectHeaders.h"
AutoScaler::AutoScaler() {
    mode = 0;
    step = 2;
    baseCapacity = 2;
    highCount = 0;
    lowCount = 0;
    cooldown = 0;
    verticalUp = horizontalUp = verticalDown = horizontalDown = 0;
}

void AutoScaler::setBaseCapacity(int cap) {
    baseCapacity = cap;
}

void AutoScaler::setMode(int m) {
    mode = m;
}

void AutoScaler::setStep(int s) {
    step = s;
}

int AutoScaler::getStep() {
    return step;
}

string AutoScaler::modeName() {
    if (mode == 1) return "Only Vertical";
    if (mode == 2) return "Only Horizontal";
    return "Automatic (vertical first, then horizontal)";
}

string AutoScaler::getType() { return type; }
string AutoScaler::getMessage() { return message; }

// total load of active servers / total capacity of active servers
int AutoScaler::utilization(vector<Server*>& servers, int waiting) {
    int load = 0, capacity = 0;
    for (int i = 0; i < (int)servers.size(); i++) {
        if (servers[i]->isActive()) {
            load += servers[i]->getLoad();
            capacity += servers[i]->getCapacity();
        }
    }
    if (capacity == 0) {            // no server is working
        if (waiting > 0) return 100;
        return 0;
    }
    return load * 100 / capacity;
}

bool AutoScaler::check(vector<Server*>& servers, int waiting) {
    if (cooldown > 0) {
        cooldown--;
        return false;
    }

    int util = utilization(servers, waiting);

    if (util > 80) highCount++;
    else highCount = 0;

    if (util < 30 && waiting == 0) lowCount++;
    else lowCount = 0;

    bool changed = false;
    if (highCount >= 3) {
        changed = scaleUp(servers, util);
        highCount = 0;
    } else if (lowCount >= 10) {
        changed = scaleDown(servers, util);
        lowCount = 0;
    }

    if (changed)
        cooldown = 5;
    return changed;
}

bool AutoScaler::scaleUp(vector<Server*>& servers, int util) {
    // vertical: the busiest server that can still grow (not in "only horizontal" mode)
    Server* best = NULL;
    for (int i = 0; i < (int)servers.size() && mode != 2; i++) {
        Server* s = servers[i];
        if (!s->isActive() || s->getCapacity() + step > MAX_CAPACITY)
            continue;
        if (best == NULL || s->loadRatio() > best->loadRatio())
            best = s;
    }

    if (best != NULL) {
        int oldCap = best->getCapacity();
        best->increaseCapacity(step, MAX_CAPACITY);
        verticalUp++;
        type = "V-SCALE UP";
        message = "Server " + to_string(best->getId()) + " capacity " + to_string(oldCap) +
                  " -> " + to_string(best->getCapacity()) + " | utilization " + to_string(util) + "%";
        return true;
    }

    // "only vertical" mode never adds servers
    if (mode == 1)
        return false;

    // horizontal: add a new server
    if (servers.size() >= 10)
        return false;

    int newId = 0, newCap = -1;
    bool sameCapacity = true;
    for (int i = 0; i < (int)servers.size(); i++) {
        if (servers[i]->getId() > newId)
            newId = servers[i]->getId();
        if (!servers[i]->isActive())
            continue;
        if (newCap == -1)
            newCap = servers[i]->getCapacity();
        else if (servers[i]->getCapacity() != newCap)
            sameCapacity = false;
    }
    newId++;
    // same capacity as the others so all servers stay equal
    if (newCap == -1 || !sameCapacity)
        newCap = baseCapacity;

    servers.push_back(new Server(newId, newCap));
    horizontalUp++;
    type = "H-SCALE UP";
    message = "Server " + to_string(newId) + " added (capacity " + to_string(newCap) +
              ") | utilization " + to_string(util) + "%";
    return true;
}

bool AutoScaler::scaleDown(vector<Server*>& servers, int util) {
    int activeCount = 0;
    for (int i = 0; i < (int)servers.size(); i++)
        if (servers[i]->isActive())
            activeCount++;

    // horizontal: remove an empty server (keep at least 2)
    if (activeCount > 2 && mode != 1) {
        for (int i = servers.size() - 1; i >= 0; i--) {
            if (servers[i]->isActive() && servers[i]->getLoad() == 0) {
                type = "H-SCALE DOWN";
                message = "Server " + to_string(servers[i]->getId()) + " removed | utilization " +
                          to_string(util) + "%";
                delete servers[i];
                servers.erase(servers.begin() + i);
                horizontalDown++;
                return true;
            }
        }
    }

    // vertical: reduce the capacity of a server that was made bigger
    for (int i = 0; i < (int)servers.size() && mode != 2; i++) {
        Server* s = servers[i];
        if (!s->isActive())
            continue;
        int oldCap = s->getCapacity();
        if (s->decreaseCapacity(step, baseCapacity)) {
            verticalDown++;
            type = "V-SCALE DOWN";
            message = "Server " + to_string(s->getId()) + " capacity " + to_string(oldCap) +
                      " -> " + to_string(s->getCapacity()) + " | utilization " + to_string(util) + "%";
            return true;
        }
    }
    return false;
}
