all: object compile

object:
	gcc -c mymalloc.c

compile:
	gcc memgrind.c mymalloc.o -o memgrind

clean:
	rm -rf memgrind mymalloc.o