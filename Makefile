targets := libuthread.a test_preempt uthread_join uthread_yield uthread_hello test_queue
lib := libuthread/libuthread.a
linklib := -l uthread -L ./libuthread
CC := gcc
CFLAGS := -Wall -Werror -g

all: $(targets)

libuthread.a: libuthread/*
	cd libuthread && make
test_preempt: test_preempt.c $(lib)
	$(CC) $^ -o $@ $(CFLAGS) $(linklib)
uthread_join: uthread_join.c $(lib)
	$(CC) $^ -o $@ $(CFLAGS) $(linklib)
uthread_yield: uthread_yield.c $(lib)
	$(CC) $^ -o $@ $(CFLAGS) $(linklib)
uthread_hello: uthread_hello.c $(lib)
	$(CC) $^ -o $@ $(CFLAGS) $(linklib)
test_queue: test_queue.c libuthread/queue.c
	$(CC) $^ -o test_queue $(CFLAGS)
clean:
	rm -rf *.o *.out core $(targets) && cd libuthread && make clean
