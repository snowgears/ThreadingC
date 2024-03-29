#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "libuthread/queue.h"

/* Callback function that increments items by a certain value */
static int inc_item(queue_t q, void *data, void *arg)
{
	int *a = (int*)data;
	int inc = (intptr_t)arg;

	*a += inc;

	return 0;
}

/* Callback function that finds a certain item according to its value */
static int find_item(queue_t q, void *data, void *arg)
{
	int *a = (int*)data;
	int match = (intptr_t)arg;

	if (*a == match)
	return 1;

	return 0;
}

int main(int argc, char *argv[])
{
	queue_t q;
	int data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	int i;
	int *ptr;

	/* Initialize the queue and enqueue items */
	q = queue_create();
	for (i = 0; i < 10; i++){
		queue_enqueue(q, &data[i]);
	}
	// Test len
	int len = queue_length(q);
	assert(len == 10);

	// Test dequeue
	queue_dequeue(q, (void**)&ptr);
	printf("Dequeued: %d\n", *ptr);

	// Test len after dequeue
	len = queue_length(q);
	assert(len == 9);

	queue_iterate(q, find_item, (void*)5, (void**)(&ptr) );

	assert(data[0] == 1);
	assert(data[9] == 10);
	assert(data[4] == 5);

	/* Add value '1' to every item of the queue */
	queue_iterate(q, inc_item, (void*)1, NULL);
	assert(data[1] == 3);
	assert(data[9] == 11);

	/* Find and get the item which is equal to value '5' */
	queue_iterate(q, find_item, (void*)5, (void**)(&ptr) );
	assert(*ptr == 5);
	
	// Test for null args
	assert(queue_destroy(NULL) == -1);
	assert(queue_enqueue(NULL, NULL) == -1);
	
	// Can't destroy non-empty queue
	assert(queue_destroy(q) == -1);
	
	// Empty the queue
	len = queue_length(q);
	for (i = 0; i < len; i++){
		queue_delete(q, &data[i+1]);
	}
	
	// Must be empty now
	assert(queue_length(q) == 0);
	
	// Test destroy
	assert(queue_destroy(q) == 0);
	
	
}
