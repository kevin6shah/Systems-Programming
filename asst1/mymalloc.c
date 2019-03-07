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
void* createMetadata(size_t size, size_t x, void* ptr) {
	if (x == size || (x-size) <= metadata_size) {	// If there is a perfect fit or close fit (*** GOTTA CHECK ON THE CLOSE FIT PART ***)
		*(short*)(ptr-metadata_size) = x;
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
		int i = metadata_size*2;
		void *ptr = &myblock[metadata_size*2];
		if (FREE_FIRST_BLOCK > 0) {	// If FREE_FIRST_BLOCK is positive, then the first block is free
			if (FREE_FIRST_BLOCK >= x) {
				return createMetadata(x, (size_t) FREE_FIRST_BLOCK, &myblock[metadata_size*2]);
			}
		}
		short sum = metadata_size;
		while (sum <= (4096-metadata_size)) {
			short size = *(short*)(ptr-metadata_size);
			if (size < 0) {			// Look into Free Space
				size = abs(size);
				if (size >= x) {		// Check if the free space is large enough for our malloc
					return createMetadata(x, (size_t) size, ptr);
				}
			}
			size = abs(size);
			ptr = ptr+size+metadata_size;
			sum = sum + metadata_size + abs(size);
		}
	}
	return NULL;
}

void print() {
	if (MAGIC_NUMBER == 31735) {
		int sum = metadata_size;
		printf("\n");
		printf("Magic Number %d\n", MAGIC_NUMBER);
		int i = metadata_size*2;
		void* ptr = &myblock[metadata_size*2];
		while (sum <= 4094) {
			short size = *(short*)(ptr-metadata_size);
			sum = sum + metadata_size + abs(size);
			printf("Size: %d ", abs(size));
			if (size > 0) printf("Data\n");
			if (size < 0) printf("Free Space\n");
			size = abs(size);
			ptr = ptr+size+metadata_size;
		}
		printf("Sum: %d", sum);
		printf("\n");
	} else printf("Incorrect Malloc Structure\n");
}

void myfree(void *ptr, char* FILE, int LINE) {
	printf("Success My Free\n");
}