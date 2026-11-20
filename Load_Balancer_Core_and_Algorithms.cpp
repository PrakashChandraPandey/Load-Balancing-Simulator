#include "ProjectHeaders.h"
LoadBalancer::LoadBalancer(vector<int> capacities, Logger* log, bool testMode) {
    this->log = log;
    completed = NULL;
    time = 0;
    generated = rejected = failedRequests = serverFailures = algorithmChanges = 0;
    generator.setTestMode(testMode);

    string msg = "Servers created with capacities:";
    int smallest = capacities[0];
    for (int i = 0; i < (int)capacities.size(); i++) {
        servers.push_back(new Server(i + 1, capacities[i]));
        msg += " " + to_string(capacities[i]);
        if (capacities[i] < smallest)
            smallest = capacities[i];
    }
    scaler.setBaseCapacity(smallest);
    if (testMode)
        msg += " (TEST MODE: similar request times)";
    log->write(time, "SETUP", msg);

    selector.choose(servers);
    log->write(time, "ALGORITHM", selector.getCurrent()->name() + " selected - " + selector.getReason());
}

LoadBalancer::~LoadBalancer() {
    for (int i = 0; i < (int)servers.size(); i++)
        delete servers[i];
    freeList(completed);
}

string LoadBalancer::info(Request r) {
    return "Request " + to_string(r.id) + " (" + r.path + ", " + to_string(r.time) + " sec)";
}

Server* LoadBalancer::findServer(int id) {
    for (int i = 0; i < (int)servers.size(); i++)
        if (servers[i]->getId() == id)
            return servers[i];
    return NULL;
}

// request generation: new requests go into the circular queue
void LoadBalancer::generateRequests(int n) {
    for (int i = 0; i < n; i++) {
        Request r = generator.next(time);
        generated++;
        selector.addTime(r.time);

        if (!waiting.enqueue(r)) {
            rejected++;
            log->write(time, "REJECTED", info(r) + " - queue is full");
        } else {
            log->write(time, "GENERATED", info(r) + " -> waiting queue");
        }
        assignRequests();
    }
}

// takes requests from the front of the queue and gives them to servers
void LoadBalancer::assignRequests() {
    Algorithm* algo = selector.getCurrent();

    while (!waiting.isEmpty()) {
        Server* s = algo->select(servers);
        if (s == NULL)               // every server is full or failed
            return;

        Request r = waiting.dequeue();
        s->assign(r, time);
        log->write(time, "ASSIGNED", info(r) + " -> Server " + to_string(s->getId()) +
                   " | " + algo->name() + ": " + algo->reason);
    }
}

void LoadBalancer::processServers() {
    for (int i = 0; i < (int)servers.size(); i++) {
        vector<Request> done = servers[i]->process();
        for (int j = 0; j < (int)done.size(); j++) {
            completed = addNode(completed, done[j], servers[i]->getId());
            log->write(time + 1, "COMPLETED", "Request " + to_string(done[j].id) + " finished on Server " +
                       to_string(servers[i]->getId()) + " | waited " + to_string(waitTime(done[j])) + " sec");
        }
    }
}

void LoadBalancer::oneSecond() {
   
    assignRequests();

    // 2. auto scaling
    if (scaler.check(servers, waiting.size()))
        log->write(time, scaler.getType(), scaler.getMessage());

    // 3. check the algorithm every 10 seconds
    if (time > 0 && time % 10 == 0) {
        string oldName = selector.getCurrent()->name();
        if (selector.choose(servers)) {
            algorithmChanges++;
            log->write(time, "ALGORITHM", oldName + " -> " + selector.getCurrent()->name() +
                       " - " + selector.getReason());
        }
    }


    assignRequests();
    processServers();
    time++;
}

void LoadBalancer::run(int seconds) {
    for (int i = 0; i < seconds; i++)
        oneSecond();
}

// runs until the queue is empty and no server has work 
int LoadBalancer::runUntilDone() {
    int seconds = 0;
    while (seconds < 1000) {
        if (getWaiting() == 0 && getProcessing() == 0)
            break;
        oneSecond();
        seconds++;
    }
    return seconds;
}

void LoadBalancer::setScaling(int mode, int step) {
    scaler.setMode(mode);
    scaler.setStep(step);
    log->write(time, "SETTING", "Scaling: " + scaler.modeName() +
               " | capacity change per vertical step: " + to_string(step));
}

void LoadBalancer::summary() {
    string avg = to_string(averageWait(completed));
    avg = avg.substr(0, avg.find('.') + 3);   

    log->writeLine("---------- SUMMARY ----------");
    log->writeLine("Total time          : " + to_string(time) + " sec");
    log->writeLine("Requests generated  : " + to_string(generated));
    log->writeLine("Completed           : " + to_string(countNodes(completed)));
    log->writeLine("Rejected            : " + to_string(rejected));
    log->writeLine("Failed (moved > 3)  : " + to_string(failedRequests));
    log->writeLine("Average waiting time: " + avg + " sec");
    log->writeLine("Algorithm changes   : " + to_string(algorithmChanges));
    log->writeLine("Vertical scaling    : " + to_string(scaler.verticalUp) + " up, " +
                   to_string(scaler.verticalDown) + " down");
    log->writeLine("Horizontal scaling  : " + to_string(scaler.horizontalUp) + " added, " +
                   to_string(scaler.horizontalDown) + " removed");
    log->writeLine("Server failures     : " + to_string(serverFailures));

    string perServer = "Requests per server :";
    for (int i = 0; i < (int)servers.size(); i++)
        perServer += " S" + to_string(servers[i]->getId()) + "=" + to_string(servers[i]->getHandled());
    log->writeLine(perServer);

    sortByWait(completed);
    string longest = "5 longest waits     :";
    Node* p = completed;
    for (int i = 0; i < 5 && p != NULL; i++) {
        longest += " R" + to_string(p->data.id) + "(" + to_string(waitTime(p->data)) + "s)";
        p = p->next;
    }
    log->writeLine(longest);
    log->writeLine("-----------------------------");
}

// ---------- used by the GUI ----------

vector<Server*> LoadBalancer::getServers() { return servers; }
CircularQueue* LoadBalancer::getQueue() { return &waiting; }
string LoadBalancer::getAlgorithmName() { return selector.getCurrent()->name(); }
string LoadBalancer::getAlgorithmReason() { return selector.getReason(); }
int LoadBalancer::getUtilization() { return scaler.utilization(servers, waiting.size()); }
string LoadBalancer::getScalingMode() { return scaler.modeName(); }
int LoadBalancer::getScalingStep() { return scaler.getStep(); }
int LoadBalancer::getTime() { return time; }
int LoadBalancer::getGenerated() { return generated; }
int LoadBalancer::getWaiting() { return waiting.size(); }
int LoadBalancer::getCompleted() { return countNodes(completed); }

int LoadBalancer::getProcessing() {
    int total = 0;
    for (int i = 0; i < (int)servers.size(); i++)
        total += servers[i]->getLoad();
    return total;
}
