#include "mymalloc.h"

void testA();
void testB();

int main(int argc, char** argv) {
	// Lets starts working on the memgrind now and make sure we only use
	// git push and git pull, no more git push origin master stuff
	testA();
}

void testA() {
	int i;
	for (i = 0; i < 150; i++) {
		char* ptr = (char*) malloc(1);
		free(ptr);
	}
	print();
}

void testB() {
	char **ptr;
	int i;
	for (i = 0; i < 150; i++) {
		// I dont really know what he wants us to do
	}
}