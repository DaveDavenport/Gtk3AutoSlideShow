LIBS=$(shell pkg-config --libs gtk+-3.0)
CFLAGS=$(shell pkg-config --cflags gtk+-3.0)

all: viewer


viewer: viewer.c
	$(CC) -o $@ $^ ${CFLAGS} ${LIBS}

clean:
	rm viewer

.PHONY: clean
