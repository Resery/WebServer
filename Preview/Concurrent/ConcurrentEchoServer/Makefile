CC = g++
CFLAGS = -g -o

all: client server

client: Client.cpp
	$(CC) $(CFLAGS) Client Client.cpp

server: Server.cpp
	$(CC) $(CFLAGS) Server Server.cpp

clean:
	rm -rf Client Server