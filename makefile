CC=gcc
CFLAGS=-lm -Wall -Wextra -Wformat-security -Wswitch-default -O3 -Ofast
OBJ = list.o stel.o
default: stel5

%.o: %.c stel.h list.h
	$(CC) -c -o $@ $< $(CFLAGS)

stel5: stel5.c $(OBJ)
	$(CC) $@.c -o $@ $(OBJ) $(CFLAGS)

clean:
	-rm -f *.o
	-rm -f stel2a