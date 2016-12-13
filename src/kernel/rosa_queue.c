#include "kernel/rosa_queue.h"
#include "kernel/rosa_ker.h"
#include "stdlib.h"

//1 - error allocating memory
//0 - OK
int queue_init(queue *q)
{
	q->count = 0;
	q->initial_size = 16;
	q->max_size = MAX_TASK_NUMBER;
	q->size = q->initial_size;
	//q->heaparr = (queue_item *) calloc(q->initial_size, sizeof(queue_item));
	q->heaparr = (queue_item *) malloc(sizeof(queue_item) * q->initial_size);
	if(!q->heaparr) {
		return 1;
	}
	return 0;
}

void max_heapify(queue_item *data, int count, int loc) {
	int left = 2*(loc) + 1;
	int right = left + 1;
	int largest = loc;
	
	if (left <= count && data[left].value > data[largest].value) {
		largest = left;
	}
	if (right <= count && data[right].value > data[largest].value) {
		largest = right;
	}

	queue_item temp;
	if(largest != loc) {
		temp = data[loc];
		data[loc] = data[largest];
		data[largest] = temp;
		max_heapify(data, largest, count);
	}
}

void min_heapify(queue_item *data, int count, int loc) {
	int left = 2*(loc) + 1;
	int right = left + 1;
	int smallest = loc;
	
	if (left <= count && data[left].value > data[smallest].value) {
		smallest = left;
	}
	if (right <= count && data[right].value > data[smallest].value) {
		smallest = right;
	}

	queue_item temp;
	if(smallest != loc) {
		temp = data[loc];
		data[loc] = data[smallest];
		data[smallest] = temp;
		max_heapify(data, smallest, count);
	}
}

//return 2 if there is no more room
//return 1 if memory allocation fails
// return 0 if everything is ok
int queue_push(queue *q, queue_item *new_item, int minimax)
{
	int index;
	int parent;

	// Resize the heap if it is too small to hold all the data
	if (q->count == q->size)
	{
		if(q->size == q->max_size){
			//no more room!
			return 2;
		}
		q->size *= 2;
		q->heaparr = realloc(q->heaparr, sizeof(queue_item) * q->size);
		if (!q->heaparr) return 1; // Exit if the memory allocation fails
	}

	index = q->count++; // First insert at last of array

	// Find out where to put the element and put it
	for(;index; index = parent)
	{
		parent = (index - 1) / 2;
		if(minimax){
			if (q->heaparr[parent].value >= new_item->value) break;
		}
		else{
			if (q->heaparr[parent].value <= new_item->value) break;
		}
		q->heaparr[index] = q->heaparr[parent];
	}
	q->heaparr[index] = *new_item;
	return 0;
}

void queue_display(queue *q) {
	int i;
	for(i=0; i<q->count; i++) {
		//printf("|%d|", q->heaparr[i].value);
		usartWriteTcb(USART, q->heaparr[i].task_tcb);
	}
	//printf("\n");
}

//If the removed task's tcb is NULL, an allocation error occurred
//minimax==1 - max heap
//minimax==0 - min heap
queue_item queue_delete_first_node(queue *q, int minimax)
{
	queue_item removed;
	removed.task_tcb = NULL;
	queue_item last = q->heaparr[--q->count];


	if ((q->count < (q->size / 2)) && (q->size > q->initial_size))
	{
		q->size /= 2;
		q->heaparr = realloc(q->heaparr, sizeof(queue_item) * q->size);
		if (!q->heaparr) return removed; // Exit if the memory allocation fails
	}
	removed = q->heaparr[0];
	q->heaparr[0] = last;
	if(minimax){
		max_heapify(q->heaparr, q->count, 0);
	}
	else{
		min_heapify(q->heaparr, q->count, 0);
	}
	return removed;
}

//Returns NULL if the queue is empty
queue_item* queue_getFirst(queue* q)
{
	//queue_item* item;
	//item->task_tcb = NULL;
	//item->value = 0;
	if(q->count == 0) return NULL;
	return q->heaparr;
}

void queue_empty(queue *q, int minimax) {
	while(q->count != 0) {
		//printf("<<%d", queue_delete_first_node(q));
		queue_delete_first_node(q, minimax);
	}
}

//return -1 if the task is not found
//otherwise return the index of the task
//DUMMY SEARCH FOR NOW
int queue_getPosition(queue *q, tcb *task){
	int i;
	for (i = 0; i < q->count; i++){
		if(q->heaparr[i].task_tcb == task){
			return i;
		}
	}
	return -1;
}

//If the removed task's tcb is NULL, an allocation error occurred
//minimax==1 - max heap
//minimax==0 - min heap
queue_item queue_remove(queue *q, tcb *task, int minimax)
{
	queue_item removed;
	removed.task_tcb = NULL;
	queue_item last = q->heaparr[--q->count];


	if ((q->count < (q->size / 2)) && (q->size > q->initial_size))
	{
		q->size /= 2;
		q->heaparr = realloc(q->heaparr, sizeof(queue_item) * q->size);
		if (!q->heaparr) return removed; // Exit if the memory allocation fails
	}
	int position = queue_getPosition(q, task);
	removed = q->heaparr[position];
	q->heaparr[position] = last;
	if(minimax){
		max_heapify(q->heaparr, q->count, position);
	}
	else{
		min_heapify(q->heaparr, q->count, position);
	}
	
	return removed;
}

// 0 everything is ok
// 1 overflow occurred
int queue_decreaseValues(queue *q, unsigned int offset){
	int i;
	unsigned int temp[q->count];
	for (i = 0; i < q->count; i++){
		temp[i] = q->heaparr[i].value;
		temp[i] -= offset;
		if(temp[i] > q->heaparr[i].value) return 1;
	}
	for (i = 0; i < q->count; i++){
		q->heaparr[i].value = temp[i];
	}
	return 0;	
}