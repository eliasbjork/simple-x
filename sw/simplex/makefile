# This makefile is used to build and test the simplex program for a host machine

CC = gcc
CFLAGS = -g -O0 -Wall -fsanitize=address
LDFLAGS = -lm

OBJS = simplex.o util.o


all: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) -o simplex

test: all
	pytest -v

clean:
	rm -rf simplex *.o