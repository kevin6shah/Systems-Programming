
#include "mymalloc.h"
#include <math.h>
#define metadata_size 2;

void* mymalloc(size_t x, char* FILE, int LINE) {
    
    int actual_size = x + metadata_size;
    
    short first_two_bytes;
    int firstmalloc = 0; //keeps track if it is the first time malloc has occurred.
    memcpy(&first_two_bytes, myblock, sizeof(short));
    if (first_two_bytes = 31767){
        firstmalloc = 1;
    }
    
    
    //will set the first two bytes to magic number, and the next two bytes as metadata
    //to handle the rest of the block.
    
    if (firstmalloc == 0){ //malloc has not yet occured, initialize everything.
        short magic_number = 31767;
        memcpy(block, &magic_number, sizeof(short));
        short first_blocksize = -4092; //negative number reflects that it is unoccupied
        memcpy(block+2, &first_blocksize, sizeof(short));
    }
    
    int i = 2; //it is pointing to the first byte of first metadata.
    int block_traversed = 2; //greatest value this can have is 4094.
    while (i < 4092){
        short metadata;
        memcpy(&metadata, block + i, sizeof(short));
        if (metadata < 0 && abs((int)metadata) > actual_size){ //there is enough and empty space to malloc here
            
        }
    }
    
    
}

void myfree(void *ptr, char* FILE, int LINE) {
    
}

