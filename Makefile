CFLAGS = -Wall -std=gnu11
LDFLAGS = -lasound -lncurses -lpthread -lm

all:
	gcc $(CFLAGS) *.c -o acratone $(LDFLAGS)
