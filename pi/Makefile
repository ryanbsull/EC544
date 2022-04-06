CC = gcc
CFLAGS = -O3 -o
FILES = server client

all: $(FILES)

server: server.c
	$(CC) $? $(CFLAGS) $@

client: client.c
	$(CC) $? $(CFLAGS) $@

clean:
	rm -rf $(FILES)