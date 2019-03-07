#include "mymalloc.h"
#define metadata_size sizeof(short)
#define MAGIC_NUMBER *(short*)myblock	// Retrieves the Magic Number
#define MALLOCED_FIRST_BLOCK *(short*)&myblock[metadata_size]	// Gets the size of the first block if it's malloced
#define FREE_FIRST_BLOCK -1*(*(short*)&myblock[metadata_size])	// Gets the size of the first block if it's free

void* initialize(size_t x) {	// Initializes by allocating for it and returns the pointer
	MAGIC_NUMBER = 31735;
	MALLOCED_FIRST_BLOCK = x;
	// Below: Creates a new metadata after the malloced region, and makes the size of it negative since its "free"
	*(short*)&myblock[(metadata_size*2) + MALLOCED_FIRST_BLOCK] = (MALLOCED_FIRST_BLOCK + (metadata_size*3)) - 4096;
	return &myblock[metadata_size*2];
}

/*
 * Creates a metadata for the remaining free space
 * and returns the pointer for the allocated space.
 */
void* createMetadata(short size, size_t x, void* ptr) {
	if (x == size || (x-size) <= metadata_size) {	// If there is a perfect fit or close fit (*** GOTTA CHECK ON THE CLOSE FIT PART ***)
		return ptr;
	}
	short actualSize = x - (size + metadata_size);
	*(short*)(ptr-metadata_size) = size;
	*(short*)(ptr+size) = -1*actualSize;
	return ptr;
}

void* mymalloc(size_t x, char* FILE, int LINE) {
	if (MAGIC_NUMBER != 31735) {	// Checks for the Magic Number
		if (!(x <= 0)) {
			return initialize(x);
		} return NULL;
	} else {
		if (x <= 0) {
			return NULL;	// Error Check
		}
		if (FREE_FIRST_BLOCK > 0) {	// If FREE_FIRST_BLOCK is positive, then the first block is free
			
		}
	}
	return NULL;
}

void myfree(void *ptr, char* FILE, int LINE) {
	printf("Success My Free\n");
}