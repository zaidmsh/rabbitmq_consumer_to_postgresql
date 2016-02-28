RABBITMQ=/usr/local/Cellar/rabbitmq-c/0.7.1/
CC=gcc
CFLAGS=-Wall -g -O3 -I$(RABBITMQ)/include -I/Applications/Postgres.app/Contents/Versions/9.5/include/
LDFLAGS=
LDLIBS=-L$(RABBITMQ)/lib -lrabbitmq -L/Applications/Postgres.app/Contents/Versions/9.5/lib -lpq

OBJS=main.o utils.o mqconsumer.o

all: main

main: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

.PHONY: clean
clean:
	-rm main *.o
