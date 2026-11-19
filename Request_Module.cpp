// Request module
#include "data_structures.h"
#include <string.h>

Request makeRequest(int id, const char *path, int time, int arrival)
{
    Request r;
    r.id = id;
    strcpy(r.path, path);
    r.time = time;
    r.remaining = time;
    r.arrival = arrival;
    r.start = -1;
    r.moves = 0;
    return r;
}

int waitTime(Request r)
{
    if (r.start == -1)
        return 0;
    return r.start - r.arrival;
}
