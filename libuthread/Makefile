libuthread.a: queue.o
	ar rcs libuthread.a queue.o
queue.o: queue.c
	gcc queue.c -c -o queue.o -Wall -Werror -g
clean:
	rm -rf *.o libuthread.a