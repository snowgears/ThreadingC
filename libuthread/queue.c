#include <stdint.h>
#include <stdlib.h>

#include "queue.h"

struct queue {
	/* TODO */
};

queue_t queue_create(void)
{
	return NULL;
}

int queue_destroy(queue_t queue)
{
	return 0;
}

int queue_enqueue(queue_t queue, void *data)
{
	return 0;
}

int queue_dequeue(queue_t queue, void **data)
{
	return 0;
}

int queue_delete(queue_t queue, void *data)
{
	return 0;
}

int queue_iterate(queue_t queue, queue_func_t func, void *arg, void **data)
{
	return 0;
}

int queue_length(queue_t queue)
{
	return 0;
}

