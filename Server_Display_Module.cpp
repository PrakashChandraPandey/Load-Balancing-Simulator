#include "ProjectHeaders.h"
void LoadBalancer::showServers() {
    cout << "\n========== STATUS AT " << time << " sec ==========" << endl;
    cout << "Algorithm   : " << getAlgorithmName() << " (selected automatically)" << endl;
    cout << "Reason      : " << getAlgorithmReason() << endl;
    cout << "Scaling     : " << getScalingMode() << " | vertical step: " << getScalingStep() << endl;
    cout << "Utilization : " << getUtilization() << "%" << endl << endl;
    for (int i = 0; i < (int)servers.size(); i++)
        servers[i]->display();
}
