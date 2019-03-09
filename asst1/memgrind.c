#include "mymalloc.h"

int testA();
int testB();

int main(int argc, char** argv) {
	// Lets starts working on the memgrind now and make sure we only use
	// git push and git pull, no more git push origin master stuff
	if (testA()) {
		printf("TestA Success!\n");
	}
	void *ptr = malloc(1);
	void *ptr1 = malloc(1);
	void *ptr2 = malloc(1);
	void *ptr3 = malloc(1);
	void *ptr4 = malloc(1);
	void *ptr5 = malloc(1);
	free(ptr);
	print();
	free(ptr1);
	print();
	free(ptr2);
	print();
	free(ptr3);
	free(ptr4);
	free(ptr5);
	printf("DONELINEE\n");
	if (testB()) {
		printf("TestB Success!\n");
	}
}


int testA() {
	int i;
	for (i = 0; i < 150; i++) {
		char* ptr = (char*) malloc(1);
		if (ptr == NULL) return 0;
		free(ptr);
	}
	print();
	return 1;
}
 



int testB() {
	char *ptr[150];
	int i, j, condition;
	for (i = 0; i < 150; i++) {
		ptr[i] = (char*) malloc(1);
		if (ptr[i] == NULL) return 0;
		if (i == 49 || i == 99 || i == 149) {
			if (i == 49) {
				j = 0;
				condition = 50;
			} else if (i == 99) {
				j = 50;
				condition = 100;
			} else {
				j = 100;
				condition = 150;
			}
			for (; j < condition; j++) {
				free(ptr[j]);
			}
		}
	}
	print();
	return 1;
}

