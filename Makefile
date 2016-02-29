RABBITMQ=/usr/local/Cellar/rabbitmq-c/0.7.1/
CC=gcc
CFLAGS=-Wall -g -O3 `pkg-config --cflags libpq`
LDFLAGS=
LDLIBS=-lrabbitmq -lpq 

OBJS=main.o utils.o mq.o pg.o

all: main

main: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

.PHONY: clean
clean:
	-rm main *.o
