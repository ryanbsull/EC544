CC = gcc
CFLAGS = -O3 -o
FILES = encrypt

all: $(FILES)

encrypt: encrypt.c
	$(CC) $? $(CFLAGS) $@

clean:
	rm -rf $(FILES)
