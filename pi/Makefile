CC = gcc
CFLAGS = -O3 -o
INCLUDES = ../encryption/encrypt.c
FILES = server client

all: $(FILES)

server: server.c
	$(CC) $? $(INCLUDES) $(CFLAGS) $@ -lcrypto -lssl

client: client.c
	$(CC) $? $(CFLAGS) $@

clean:
	rm -rf $(FILES) *.pem
