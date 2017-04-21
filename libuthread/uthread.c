#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "context.h"
#include "preempt.h"
#include "queue.h"
#include "uthread.h"

void uthread_yield(void)
{
}

uthread_t uthread_self(void)
{
	return 0;
}

int uthread_create(uthread_func_t func, void *arg)
{
	return 0;
}

void uthread_exit(int retval)
{
}

int uthread_join(uthread_t tid, int *retval)
{
	return 0;
}

