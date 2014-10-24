CC = gcc
OBJS = serverEpoll.c clientEpoll.c
CFLAG = -std=c99

all: c s

c:clientEpoll.o
	$(CC) $^ $(CFLAG) -o $@
s:serverEpoll.c
	$(CC) $^ $(CFLAG) -o $@
clean:
	rm *.o c s
