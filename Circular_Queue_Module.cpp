#include "ProjectHeaders.h"

CircularQueue::CircularQueue() {
    q = createQueue(QUEUE_SIZE);
}

CircularQueue::~CircularQueue() {
    freeQueue(q);
}

bool CircularQueue::isEmpty() {
    return queueEmpty(q);
}

bool CircularQueue::isFull() {
    return queueFull(q);
}

int CircularQueue::size() {
    return q->count;
}

bool CircularQueue::enqueue(Request r) {
    return ::enqueue(q, r) == 1;
}


Request CircularQueue::dequeue() {
    return ::dequeue(q);
}

Request CircularQueue::at(int i) {
    return queueAt(q, i);
}

Request CircularQueue::slot(int index) {
    return q->arr[index];
}

int CircularQueue::getFront() {
    return q->front;
}

int CircularQueue::getRear() {
    return q->rear;
}
