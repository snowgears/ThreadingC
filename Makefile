make_lib: libuthread/*
	cd libuthread && make
test_preempt: test_preempt.c libuthread/libuthread.a
		gcc test_preempt.c libuthread/libuthread.a -o test_preempt -Wall -Werror -g -l uthread -L ./libuthread
uthread_join: uthread_join.c libuthread/libuthread.a
		gcc uthread_join.c libuthread/libuthread.a -o uthread_join -Wall -Werror -g -l uthread -L ./libuthread
uthread_yield: uthread_yield.c libuthread/libuthread.a
		gcc uthread_yield.c libuthread/libuthread.a -o uthread_yield -Wall -Werror -g -l uthread -L ./libuthread
uthread_hello: uthread_hello.c libuthread/libuthread.a
	gcc uthread_hello.c libuthread/queue.c libuthread/uthread.c -o uthread_hello -Wall -Werror -g -l uthread -L ./libuthread
test_queue: test_queue.c libuthread/queue.c
	gcc test_queue.c libuthread/queue.c -o test_queue -Wall -Werror -g
clean:
	rm -rf *.o test_queue *.out core uthread_hello
