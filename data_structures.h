#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include <stdio.h>
#include <stdlib.h>
typedef struct {
    int id;
    char path[16];     
    int time;          
    int remaining;     
    int arrival;      
    int start;         
    int moves;         
} Request;

Request makeRequest(int id, const char *path, int time, int arrival);
int waitTime(Request r);

// circular queue
typedef struct {
    Request *arr;
    int size;
    int front;
    int rear;
    int count;
} Queue;

inline Queue *createQueue(int size)
{
    Queue *q = (Queue *)malloc(sizeof(Queue));
    if (q == NULL) {
        printf("Memory not available\n");
        exit(1);
    }
    q->arr = (Request *)malloc(size * sizeof(Request));
    if (q->arr == NULL) {
        printf("Memory not available\n");
        exit(1);
    }
    q->size = size;
    q->front = 0;
    q->rear = -1;
    q->count = 0;
    return q;
}

inline void freeQueue(Queue *q)
{
    free(q->arr);
    free(q);
}

inline int queueEmpty(Queue *q)
{
    return q->count == 0;
}

inline int queueFull(Queue *q)
{
    return q->count == q->size;
}

inline int enqueue(Queue *q, Request r)
{
    if (queueFull(q))
        return 0;
    q->rear = (q->rear + 1) % q->size;
    q->arr[q->rear] = r;
    q->count++;
    return 1;
}

inline Request dequeue(Queue *q)
{
    Request r = q->arr[q->front];
    q->front = (q->front + 1) % q->size;
    q->count--;
    return r;
}

inline Request queueAt(Queue *q, int i)
{
    return q->arr[(q->front + i) % q->size];
}

typedef struct Node {
    Request data;
    int serverId;
    struct Node *next;
} Node;

// insert at the end, returns the head
inline Node *addNode(Node *head, Request r, int serverId)
{
    Node *newNode = (Node *)malloc(sizeof(Node));
    if (newNode == NULL) {
        printf("Memory not available\n");
        exit(1);
    }
    newNode->data = r;
    newNode->serverId = serverId;
    newNode->next = NULL;

    if (head == NULL)
        return newNode;

    Node *temp = head;
    while (temp->next != NULL)
        temp = temp->next;
    temp->next = newNode;
    return head;
}

inline int countNodes(Node *head)
{
    int count = 0;
    while (head != NULL) {
        count++;
        head = head->next;
    }
    return count;
}

inline double averageWait(Node *head)
{
    int total = 0, count = 0;
    while (head != NULL) {
        total += waitTime(head->data);
        count++;
        head = head->next;
    }
    if (count == 0)
        return 0;
    return (double)total / count;
}

inline void sortByWait(Node *head)
{
    int swapped;
    Node *p;
    if (head == NULL)
        return;
    do {
        swapped = 0;
        for (p = head; p->next != NULL; p = p->next) {
            if (waitTime(p->data) < waitTime(p->next->data)) {
                Request tempData = p->data;
                int tempId = p->serverId;
                p->data = p->next->data;
                p->serverId = p->next->serverId;
                p->next->data = tempData;
                p->next->serverId = tempId;
                swapped = 1;
            }
        }
    } while (swapped);
}

inline void freeList(Node *head)
{
    while (head != NULL) {
        Node *temp = head;
        head = head->next;
        free(temp);
    }
}

#endif
