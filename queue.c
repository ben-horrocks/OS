#include"queue.h"

PriorityQueue initialize_priority_queue()
{
	PriorityQueue queue;
	queue.size = 0;
	for (int x = 0; x < MAX_QUEUE_ITEMS; x++)
	{
		queue_item item = { -1, -1 };
		queue.items[x] = item;
	}
	return queue;
}

TID add_to_priority_queue(PriorityQueue* q, int id, int priority)
{
	if (q->size == MAX_QUEUE_ITEMS)
	{
		printf("Queue Full\n");
		return -1;
	}

	queue_item item = { id, priority };
	q->items[q->size++] = item;
}

int serve_priority_queue(PriorityQueue* q, int id)
{
	int index = -1;
	if (id == -1)
	{
		int priority = -1;
		for (int x = 0; x < q->size; x++)
		{
			if (q->items[x].priority > priority)
			{
				id = q->items[x].id;
				priority = q->items[x].priority;
				index = x;
			}
		}
	}
	else
	{
		for (int x = 0; x < q->size; x++)
		{
			if (q->items[x].id == id)
			{
				index = x;
				break;
			}
		}
	}
	if (index != -1)
	{
		do
		{
			q->items[index] = q->items[index + 1];
			index++;
		} while (index < q->size - 1);
		q->items[q->size].id = -1;
		q->items[q->size].priority = -1;
		q->size--;
	}
	return id;
}

void testqueue()
{
	PriorityQueue queue = initialize_priority_queue();
	add_to_priority_queue(&queue, 1, 1);
	add_to_priority_queue(&queue, 2, 5);
	add_to_priority_queue(&queue, 3, 10);
	add_to_priority_queue(&queue, 4, 20);
	add_to_priority_queue(&queue, 5, 99);
	add_to_priority_queue(&queue, 6, 99);
	add_to_priority_queue(&queue, 100, 2);
	printf("%d", serve_priority_queue(&queue, 100));
	while (queue.size > 0)
		printf("%d", serve_priority_queue(&queue, -1));
}
