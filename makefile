all: ish

ish:
	gcc -lm -std=c99 -g -Iinclude src/ish.c -o bin/ish

run:
	./bin/ish

clean:
	rm bin/ish
