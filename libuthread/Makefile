objs := queue.o uthread.o context.o preempt.o
CC := gcc
CFLAGS := -Wall -Werror -g


libuthread.a: $(objs)
	ar rcs $@ $^
%.o: %.c
	$(CC) $^ -c -o $@ $(CFLAGS)
clean:
	rm -rf $(objs) libuthread.a