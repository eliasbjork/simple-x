CC = gcc
CFLAGS = -g -O0 -Wall -fsanitize=address
LDFLAGS = -lm

OBJS = simplex.o util.o


all: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS)

test: all
	bash run-tests

clean:
	rm -rf a.out *.o