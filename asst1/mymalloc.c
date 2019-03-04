#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"

void* mymalloc(int x) {
	printf("Success My Malloc\n");
	void *hello;
	return hello;
}

void free(int x) {
	printf("Success My Free\n");
}