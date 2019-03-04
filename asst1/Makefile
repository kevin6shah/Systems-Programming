all: object compile execute clean

object:
	gcc -c mymalloc.c

compile:
	gcc memgrind.c mymalloc.o -o memgrind

execute:
	./memgrind

clean:
	rm -rf memgrind mymalloc.o