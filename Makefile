test_queue: test_queue.c
	gcc test_queue.c libuthread/queue.c -o test_queue -Wall -Werror -g
clean:
	rm -rf *.o test_queue *.out
