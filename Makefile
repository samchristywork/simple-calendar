CC=gcc
CFLAGS=-g -Wall -Wpedantic -Wextra `pkg-config --cflags gtk+-3.0`
LIBS=`pkg-config --libs gtk+-3.0`

all: build/simple-calendar

.PHONY: objects
objects: $(patsubst src/%.c, build/%.o, $(wildcard src/*.c))

build/simple-calendar: src/*.c src/*.h objects
	@mkdir -p build
	${CC} ${CFLAGS} build/*.o -o build/simple-calendar ${LIBS}

build/%.o: src/%.c src/*.h
	@mkdir -p build
	${CC} ${CFLAGS} -c $< -o $@ ${LIBS}

run:
	make && ./build/simple-calendar

clean:
	rm -rf build
