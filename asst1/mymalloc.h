#ifndef _MY_MALLOC_H
#define _MY_MALLOC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define malloc(x) mymalloc(x, __FILE__,__LINE__)
#define free(x) myfree(x, __FILE__, __LINE__)

typedef struct {
	short size;
	unsigned int MagicNumber;
} firstCheck;

static char myblock[4096];

void *mymalloc(size_t x, char* FILE, int LINE);
void myfree(void *ptr, char* FILE, int LINE);


#endif