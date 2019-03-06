#include "mymalloc.h"

void* initialize(size_t x) {
	((firstCheck*)myblock)->size = x;
	((firstCheck*)myblock)->MagicNumber = 4163545983;
	*(short*)&myblock[sizeof(firstCheck)+x] = 4096 - (sizeof(firstCheck) + x + sizeof(short));
	return &myblock[sizeof(firstCheck)];
}

void* mymalloc(size_t x, char* FILE, int LINE) {
	if (((firstCheck*)myblock)->MagicNumber != 4163545983) {
		return initialize(x);
	} else {
		printf("Size 1: %d\n", ((firstCheck*)myblock)->size);
		printf("Size 2: %d\n", *(short*)&myblock[sizeof(firstCheck)+((firstCheck*)myblock)->size]);
	}
	return NULL;
}

void myfree(void *ptr, char* FILE, int LINE) {
	printf("Success My Free\n");
}