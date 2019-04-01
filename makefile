CC=gcc
CFLAGS=-lm -Wall -Wextra -Wformat-security -Wswitch-default -O3 -Ofast
DEPS = %.c stel.h list.h
OBJ = list.o stel.o
default: stel2a

%.o: $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

stel2a: $(OBJ)
	$(CC) $@.c -o $@ $^ $(CFLAGS)

clean:
	-rm -f *.o
	-rm -f stel2a
