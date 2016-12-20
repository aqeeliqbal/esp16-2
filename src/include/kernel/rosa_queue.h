#ifndef ROSA_QUEUE_H_
#define ROSA_QUEUE_H_

#include "kernel/rosa_def.h"

typedef struct queue_item_t {
	tcb * task_tcb; //Pointer to the task
	unsigned int value;			    //The value, either the priority or the wake time of the task
	unsigned int added_value;
} queue_item;

typedef struct queue_heap {
	int size;
	int count;
	int initial_size;
	int max_size;
	queue_item * heaparr;
} queue;

int queue_init(queue *q);
void max_heapify(queue_item *data, int count, int loc);
void min_heapify(queue_item *data, int count, int loc);
int queue_push(queue *q, queue_item *new_item, int minimax);
void queue_display(queue *q);
queue_item queue_delete_first_node(queue *q, int minimax);
queue_item* queue_getFirst(queue* q);
void queue_empty(queue *q, int minimax);
int queue_getPosition(queue *q, tcb *item);
queue_item queue_remove(queue *q, tcb *task, int minimax);
int queue_decreaseValues(queue *q, unsigned int offset);
void display_number(unsigned int number);


#endif /* ROSA_QUEUE_H_ */