#ifndef PROJECT_HEADERS_H
#define PROJECT_HEADERS_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "data_structures.h"
using namespace std;

// ==================== CIRCULAR QUEUE MODULE ====================

const int QUEUE_SIZE = 20;     

class CircularQueue {
private:
    Queue* q;

public:
    CircularQueue();
    ~CircularQueue();
    bool isEmpty();
    bool isFull();
    int size();
    bool enqueue(Request r);
    Request dequeue();
    Request at(int i);         
    Request slot(int index);   
    int getFront();
    int getRear();
};

// ==================== SERVER MODULE ====================

class Server {
private:
    int id;
    int capacity;              
    bool active;
    vector<Request> running;   
    int handled;               

public:
    Server(int id, int capacity);

    int getId();
    int getCapacity();
    int getLoad();
    int getHandled();
    bool isActive();
    bool isAvailable();
    double loadRatio();
    vector<Request> getRunning();

    void assign(Request r, int time);
    vector<Request> process();
    vector<Request> fail();
    void recover();
    bool increaseCapacity(int amount, int maxCap);
    bool decreaseCapacity(int amount, int minCap);
    void display();
};

// ==================== ALGORITHMS (Round Robin, Weighted Round Robin, Least Connections) ====================

// base class (abstract)
class Algorithm {
public:
    string reason;   
    virtual Server* select(vector<Server*>& servers) = 0;
    virtual string name() = 0;
    virtual ~Algorithm() {}
};

class RoundRobin : public Algorithm {
private:
    int next;         
public:
    RoundRobin();
    Server* select(vector<Server*>& servers);
    string name();
};

class WeightedRoundRobin : public Algorithm {
private:
    int current;      
    int given;        
public:
    WeightedRoundRobin();
    Server* select(vector<Server*>& servers);
    string name();
};

class LeastConnections : public Algorithm {
public:
    Server* select(vector<Server*>& servers);
    string name();
};


class AlgorithmSelector {
private:
    int recent[10];     
    int count;          
    int pos;            

    RoundRobin rr;
    WeightedRoundRobin wrr;
    LeastConnections lc;
    Algorithm* current;
    string reason;

public:
    AlgorithmSelector();
    void addTime(int t);
    bool choose(vector<Server*>& servers);   
    Algorithm* getCurrent();
    string getReason();
};

// ==================== AUTO-SCALER MODULE ====================

const int MAX_CAPACITY = 8;

class AutoScaler {
private:
    int mode;
    int step;
    int baseCapacity;
    int highCount;
    int lowCount;
    int cooldown;
    string type;        
    string message;     

    bool scaleUp(vector<Server*>& servers, int util);
    bool scaleDown(vector<Server*>& servers, int util);

public:
    int verticalUp, horizontalUp, verticalDown, horizontalDown;

    AutoScaler();
    void setBaseCapacity(int cap);
    void setMode(int m);
    void setStep(int s);
    int getStep();
    string modeName();
    int utilization(vector<Server*>& servers, int waiting);   
    bool check(vector<Server*>& servers, int waiting);        
    string getType();
    string getMessage();
};


class RequestGenerator {
private:
    int nextId;
    bool testMode;
    int randomBetween(int low, int high);

public:
    RequestGenerator();
    void setTestMode(bool mode);
    Request next(int time);
};

// ==================== LOGGER MODULE ====================

// Logger module: writes every event to the log file and on the screen
class Logger {
private:
    ofstream file;
    vector<string> lines;      

public:
    Logger(string fileName);
    ~Logger();
    void write(int time, string type, string message);
    void writeLine(string text);
    vector<string> getLines();
};

// ==================== LOAD BALANCER MODULE (also server failure, recovery and display) ====================


class LoadBalancer {
private:
    vector<Server*> servers;
    CircularQueue waiting;     // circular queue of waiting requests
    AlgorithmSelector selector;
    AutoScaler scaler;
    RequestGenerator generator;
    Logger* log;
    Node* completed;           // linked list of completed requests (data_structures.h)

    int time;
    int generated, rejected, failedRequests, serverFailures, algorithmChanges;

    void assignRequests();
    void processServers();
    void moveBack(Request r);
    Server* findServer(int id);
    string info(Request r);

public:
    LoadBalancer(vector<int> capacities, Logger* log, bool testMode);
    ~LoadBalancer();

    // Load_Balancer_Core_and_Algorithms.cpp
    void generateRequests(int n);
    void oneSecond();
    void run(int seconds);
    int runUntilDone();
    void setScaling(int mode, int step);
    void summary();

    // Server_Failure_Module.cpp and Server_Recovery_Module.cpp
    bool failServer(int id);
    bool recoverServer(int id);

    // Server_Display_Module.cpp and Queue_Display_Module.cpp
    void showServers();
    void showQueue();

    // used by the GUI
    vector<Server*> getServers();
    CircularQueue* getQueue();
    string getAlgorithmName();
    string getAlgorithmReason();
    int getUtilization();
    string getScalingMode();
    int getScalingStep();
    int getTime();
    int getGenerated();
    int getProcessing();
    int getWaiting();
    int getCompleted();
};

#endif
