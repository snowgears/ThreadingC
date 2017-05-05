objs := queue.o uthread.o context.o preempt.o
CC := gcc
CFLAGS := -Wall -Werror

## Debug flag
ifneq ($(D),1)
CFLAGS	+= -O2
else
CFLAGS	+= -O0
CFLAGS	+= -g
endif

# Don't print the commands unless explicitely requested with `make V=1`
ifneq ($(V),1)
Q = @
V = 0
endif

# Compile static library
libuthread.a: $(objs)
	ar rcs $@ $^

# Generic Rule for compiling lib files
%.o: %.c
	@echo "CC	$@"
	$(Q)$(CC) $^ -c -o $@ $(CFLAGS)
	
# Cleaning Rule
clean:
	@echo "CLEAN	$(shell pwd)"
	$(Q)rm -rf $(objs) libuthread.a