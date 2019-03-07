 #include "mymalloc.h"
#define metadata_size sizeof(short)
#define MAGIC_NUMBER *(short*)myblock	// Retrieves the Magic Number
#define MALLOCED_FIRST_BLOCK *(short*)&myblock[metadata_size]	// Gets the size of the first block if it's malloced
#define FREE_FIRST_BLOCK -1*(*(short*)&myblock[metadata_size])	// Gets the size of the first block if it's free
#define neg_met -2

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
		if (x > 0 && x <= 4092) {
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

// will check if ptr points to the beginning of the block
//if it points to metadata or middle of a block will return -1
//if it is outside of the bounds of the array, it will return a -1
//If it points correctly to a block (first byte), it will return the metadata of the previous block to be used later.
// if it returns a -2, the first block is being freed.
int check(void *ptr){
    if((*(short*)(ptr-metadata_size)) < 0){
        return -1;
    }
    int i = 2; //will start of pointing to the first metadata block.
    int prev_metadata = -2;
    while (i <= 4094){
        short metadata;
        memcpy(&metadata, myblock + i, metadata_size);
        void *pointer = &myblock[metadata_size + i];
        if (ptr == pointer){ //correct free passed;
            
            return (prev_metadata);
        }
        if (ptr < pointer){
            return -1;
        }
        
        prev_metadata = metadata;
        i = i + metadata_size + abs(metadata);
    }
    return -1;
}

//merges adjacent free blocks together
//to be used after free
//will merge using left-shift
//check right, if free, then merge to original ptr.
//check left, if free, merge ptr to prev.
//if prev_ptr is NULL, ptr is the first block.
void merge(void *ptr, void* prev_pointer){
    short metadata = *(short*)(ptr-metadata_size);
    int not_first_block = 0;
    int not_last_block = 0;
    short post_metadata;
    if ((ptr + metadata - 1) != myblock + 4095){
        not_last_block = 1;
        post_metadata = *(short*)(ptr + abs(metadata));
    }
    short prev_metadata;
    if (prev_pointer != NULL){
        not_first_block = 1;
        prev_metadata = *(short*)(prev_pointer-metadata_size);
    }
    
    short zero = 0;
    if(not_last_block == 1){
        if (post_metadata < 0){ //it is a free block.
            *(short*)(ptr + abs(metadata)) = zero; //sets post block metadata to zero
            *(short*)(ptr-metadata_size) = metadata + post_metadata + neg_met; // adds the post-block size to the current block + metadata size.
            metadata = *(short*)(ptr-metadata_size);
        }
    }
    if (not_first_block == 1){
        if (prev_metadata < 0){
            *(short*)(ptr - metadata_size) = zero; //sets current block meta-data to zero
            *(short*) (prev_pointer - metadata_size) = metadata + prev_metadata + neg_met; //pre-metadata block size = prev + current + metadatasize.
        }
    }
    
    
}

void myfree(void *ptr, char* FILE, int LINE) {
    int status = check(ptr);
    if (status == -1){
        printf("Error\n");
        return;
    }
    *(short *)(ptr - metadata_size) = -1 * (*(short *) (ptr-metadata_size)); //block is freed, yay.
    if (status == -2){
        merge(ptr, NULL);
        return;
    }
    void * prev_pointer = ((ptr - metadata_size) - abs(status));
    merge(ptr, prev_pointer);
    return;
    
}
