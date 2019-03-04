#include "mymalloc.h"

void* mymalloc(size_t x, char* FILE, int LINE) {
	printf("Success My Malloc\n");
	void *hello;
	return hello;
}

void myfree(void *ptr, char* FILE, int LINE) {
	printf("Success My Free\n");
}