# Target programs
programs := \
	uthread_hello.x \
	uthread_yield.x

# User-level thread library
UTHREADLIB := libuthread
UTHREADPATH := ${UTHREADLIB}
libuthread := $(UTHREADPATH)/${UTHREADLIB}.a

# Default rule
all: $(libuthread) $(programs)

# Avoid builtin rules and variables
MAKEFLAGS += -rR

# Don't print the commands unless explicitely requested with `make V=1`
ifneq ($(V),1)
Q = @
V = 0
endif

# Define compilation toolchain
CC	= gcc

# General gcc options
CFLAGS	:= -Wall -Werror
CFLAGS	+= -pipe
## Debug flag
ifneq ($(D),1)
CFLAGS	+= -O2
else
CFLAGS	+= -O0
CFLAGS	+= -g
endif

# Include path
INCLUDE := -I$(UTHREADPATH)

# Generate dependencies
DEPFLAGS = -MMD -MF $(@:.o=.d)

# Application objects to compile
objs := $(patsubst %.x,%.o,$(programs))

# Include dependencies
deps := $(patsubst %.o,%.d,$(objs))
-include $(deps)

# Rule for libuthread.a
$(libuthread):
	@echo "MAKE	$@"
	$(Q)$(MAKE) V=$(V) D=$(D) -C $(UTHREADPATH)

# Generic rule for linking final applications
%.x: %.o $(libuthread)
	@echo "LD	$@"
	$(Q)$(CC) $(CFLAGS) -o $@ $< -L$(UTHREADPATH) -luthread

# Generic rule for compiling objects
%.o: %.c
	@echo "CC	$@"
	$(Q)$(CC) $(CFLAGS) $(INCLUDE) -c -o $@ $< $(DEPFLAGS)

# Cleaning rule
clean:
	@echo "CLEAN	$(shell pwd)"
	$(Q)$(MAKE) V=$(V) D=$(D) -C $(UTHREADPATH) clean
	$(Q)rm -rf $(objs) $(deps) $(programs)

.PHONY: clean $(libuthread)

