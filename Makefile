make_lib: libuthread/*
	cd libuthread && make
uthread_hello: uthread_hello.c
	gcc uthread_hello.c libuthread/queue.c libuthread/uthread.c -o uthread_hello -Wall -Werror -g -l uthread -L ./libuthread
test_queue: test_queue.c libuthread/queue.c
	gcc test_queue.c libuthread/queue.c -o test_queue -Wall -Werror -g
clean:
	rm -rf *.o test_queue *.out core uthread_hello
