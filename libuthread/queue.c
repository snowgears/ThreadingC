#include <stdint.h>
#include <stdlib.h>

#include "queue.h"

typedef struct node* nodePtr;
typedef struct node{
	void* data;
	nodePtr next;
	nodePtr prev;
}  Node;

struct queue {
	nodePtr head; // pushed onto
	nodePtr tail; // popped from
	int size;
};


queue_t queue_create(void)
{
	queue_t queuePtr = (queue_t) malloc(sizeof(struct queue));
	queuePtr->head = NULL;
	queuePtr->tail = NULL;
	queuePtr->size = 0;

	return queuePtr;
}

int queue_destroy(queue_t queue)
{
	if(queue == NULL || queue->size != 0){
		return -1;
	}

	free(queue);
	return 0;
}

int queue_enqueue(queue_t queue, void *data)
{
	if(queue == NULL || data == NULL){
		return -1;
	}

	nodePtr newHead = (nodePtr) malloc(sizeof(Node));
	if(newHead == NULL){
		return -1;
	}
	newHead->data = data;
	newHead->next = queue->head;
	newHead->prev = NULL;

	if(queue->head != NULL){
		queue->head->prev = newHead;
	}

	if(queue->size == 0){
		queue->tail = newHead;
	}
	queue->head = newHead;
	queue->size++;

	return 0;
}

int queue_dequeue(queue_t queue, void **data)
{
	if(queue == NULL || data == NULL || queue->size == 0){
		return -1;
	}
	// Grab data
	*data = queue->tail->data;

	// Update tail
	queue->tail = queue->tail->prev;

	// Free old tail
	if(queue->tail != NULL && queue->tail->next != NULL){
		free(queue->tail->next);
	}
	queue->size--;

	// If there is a tail
	if(queue->tail != NULL){
		// Set tail's next to null
		queue->tail->next = NULL;
	}
	return 0;
}

int queue_delete(queue_t queue, void *data)
{
	if(queue == NULL || data == NULL || queue->tail->data != data){
		return -1;
	}

	// Update tail
	queue->tail = queue->tail->prev;

	// Free old tail
	if(queue->tail != NULL && queue->tail->next != NULL){
		free(queue->tail->next);
	}
	queue->size--;

	// If there is a tail
	if(queue->tail != NULL){
		// Set tail's next to null
		queue->tail->next = NULL;
	}
	return 0;

}

int queue_iterate(queue_t queue, queue_func_t func, void *arg, void **data)
{
	if(queue == NULL || func == NULL){
		return -1;
	}
	nodePtr curNode = queue->head;
	int status = 0;
	void* grabbedData = NULL;

	while(0 == status && curNode != NULL){
		grabbedData = curNode->data;
		status = func(queue, grabbedData, arg);
		curNode = curNode->next;
	}

	if(data != NULL && grabbedData != NULL){
		*data = grabbedData;
	}



	return 0;
}

int queue_length(queue_t queue)
{
	return (queue == NULL ? -1 : queue->size);
}
