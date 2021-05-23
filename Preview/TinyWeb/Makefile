CC = gcc
CFLAGS = -O2 -Wall -I .

LIB = -lpthread

all: tiny cgi

tiny: Tiny.c csapp.o
	$(CC) $(CFLAGS) -g -o Tiny Tiny.c csapp.o $(LIB)

csapp.o: csapp.c
	$(CC) $(CFLAGS) -c csapp.c

cgi:
	cd cgi-bin; make

clean:
	rm -rf *.o Tiny *~
	(cd cgi-bin; make clean)
