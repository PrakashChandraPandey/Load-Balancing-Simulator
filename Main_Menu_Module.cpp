// Console version of the simulator
#include <iostream>
#include <cstdlib>
#include <vector>
#include "ProjectHeaders.h"
using namespace std;

const bool TEST_MODE = false;   

int readNumber(string msg, int low, int high) {
    int x;
    while (true) {
        cout << msg;
        cin >> x;
        if (cin.eof())
            exit(0);
        if (cin.fail()) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Please enter a number" << endl;
        } else if (x < low || x > high) {
            cout << "Enter a number from " << low << " to " << high << endl;
        } else {
            return x;
        }
    }
}

int main() {
    srand(42);    

    try {
        Logger log("load_balancer_log.txt");

        int n = readNumber("Number of servers (2-10): ", 2, 10);
        vector<int> capacities;
        for (int i = 1; i <= n; i++)
            capacities.push_back(readNumber("Capacity of Server " + to_string(i) + " (1-8): ", 1, MAX_CAPACITY));

        LoadBalancer lb(capacities, &log, TEST_MODE);

        int choice = 0;
        while (choice != 8) {
            cout << "\n======= Multi-Algorithm Load Balancing Simulator =======" << endl;
            cout << "1. Generate Requests" << endl;
            cout << "2. Run Simulation" << endl;
            cout << "3. Run Until All Requests Finish" << endl;
            cout << "4. Show Status" << endl;
            cout << "5. Fail Server" << endl;
            cout << "6. Recover Server" << endl;
            cout << "7. Scaling Settings" << endl;
            cout << "8. Exit" << endl;
            choice = readNumber("Choice: ", 1, 8);

            if (choice == 1) {
                lb.generateRequests(readNumber("Number of requests (1-100): ", 1, 100));
            } else if (choice == 2) {
                lb.run(readNumber("Seconds to run (1-300): ", 1, 300));
            } else if (choice == 3) {
                cout << "Finished in " << lb.runUntilDone() << " seconds" << endl;
            } else if (choice == 4) {
                lb.showServers();
                lb.showQueue();
            } else if (choice == 5) {
                int id = readNumber("Server ID to fail: ", 1, 100);
                if (!lb.failServer(id))
                    cout << "Server " << id << " does not exist or has already failed" << endl;
            } else if (choice == 6) {
                int id = readNumber("Server ID to recover: ", 1, 100);
                if (!lb.recoverServer(id))
                    cout << "Server " << id << " does not exist or is already active" << endl;
            } else if (choice == 7) {
                cout << "1. Automatic (vertical first, then horizontal)" << endl;
                cout << "2. Only Vertical" << endl;
                cout << "3. Only Horizontal" << endl;
                int mode = readNumber("Scaling type: ", 1, 3) - 1;
                int step = readNumber("Capacity change per vertical step (1-4): ", 1, 4);
                lb.setScaling(mode, step);
            } else {
                lb.summary();
            }
        }
    } catch (string error) {
        cout << "Error: " << error << endl;
    }
    return 0;
}
