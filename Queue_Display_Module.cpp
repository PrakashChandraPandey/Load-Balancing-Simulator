#include "ProjectHeaders.h"
void LoadBalancer::showQueue() {
    cout << "\nWaiting requests (circular queue): front = " << waiting.getFront()
         << ", rear = " << waiting.getRear() << ", count = " << waiting.size() << endl;

    if (waiting.isEmpty()) {
        cout << "  empty" << endl;
        return;
    }
    for (int i = 0; i < waiting.size(); i++) {
        Request r = waiting.at(i);
        cout << "  [R" << r.id << " " << r.path << " " << r.time << "s]";
    }
    cout << endl;
}
