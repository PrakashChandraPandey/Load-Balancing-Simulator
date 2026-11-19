#include "ProjectHeaders.h"
Server::Server(int id, int capacity) {
    this->id = id;
    this->capacity = capacity;
    active = true;
    handled = 0;
}

int Server::getId() { return id; }
int Server::getCapacity() { return capacity; }
int Server::getLoad() { return running.size(); }
int Server::getHandled() { return handled; }
bool Server::isActive() { return active; }
vector<Request> Server::getRunning() { return running; }

// active and has a free slot
bool Server::isAvailable() {
    return active && getLoad() < capacity;
}

double Server::loadRatio() {
    return (double)getLoad() / capacity;
}

void Server::assign(Request r, int time) {
    r.start = time;
    r.remaining = r.time;
    running.push_back(r);
}


vector<Request> Server::process() {
    vector<Request> finished;
    if (!active)
        return finished;

    for (int i = 0; i < (int)running.size(); i++)
        running[i].remaining--;

    vector<Request> stillRunning;
    for (int i = 0; i < (int)running.size(); i++) {
        if (running[i].remaining <= 0) {
            finished.push_back(running[i]);
            handled++;
        } else {
            stillRunning.push_back(running[i]);
        }
    }
    running = stillRunning;
    return finished;
}

// returns the requests that were running so they can be moved
vector<Request> Server::fail() {
    vector<Request> pending = running;
    running.clear();
    active = false;
    return pending;
}

void Server::recover() {
    active = true;
}

bool Server::increaseCapacity(int amount, int maxCap) {
    if (capacity + amount > maxCap)
        return false;
    capacity = capacity + amount;
    return true;
}

bool Server::decreaseCapacity(int amount, int minCap) {
    if (capacity - amount < minCap || getLoad() > capacity - amount)
        return false;
    capacity = capacity - amount;
    return true;
}

void Server::display() {
    cout << "Server " << id << " | Capacity: " << capacity << " | Load: " << getLoad()
         << " | Status: " << (active ? "ACTIVE" : "FAILED") << endl;
    for (int i = 0; i < (int)running.size(); i++)
        cout << "     Request " << running[i].id << " (" << running[i].path << ") - "
             << running[i].remaining << " sec left" << endl;
}
