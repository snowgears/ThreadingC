libuthread.a: queue.o uthread.o context.o preempt.o
	ar rcs libuthread.a queue.o uthread.o context.o preempt.o
queue.o: queue.c
	gcc queue.c -c -o queue.o -Wall -Werror -g
uthread.o: uthread.c
	gcc uthread.c -c -o uthread.o -Wall -Werror -g
context.o: context.c
	gcc context.c -c -o context.o -Wall -Werror -g
preempt.o: preempt.c
	gcc preempt.c -c -o preempt.o -Wall -Werror -g
clean:
	rm -rf *.o libuthread.a