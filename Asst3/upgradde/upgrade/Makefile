all:
	gcc -c data.c
	gcc upgrade.c data.c -lcrypto -o update
	./update testing
	rm update data.o
