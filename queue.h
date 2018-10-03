#ifndef QUEUE_H
#define QUEUE_H

#define MAX_QUEUE_ITEMS 128
typedef int TID;
typedef int priority;

typedef struct 
{
	TID id;
	priority priority;
} queue_item;

typedef struct
{
	int size;
	queue_item items[128];
} PriorityQueue;

PriorityQueue initialize_priority_queue();
TID add_to_priority_queue(PriorityQueue* q, int id, int priority);
int serve_priority_queue(PriorityQueue* q, int id);
void testqueue();
#endif // QUEUE_H
