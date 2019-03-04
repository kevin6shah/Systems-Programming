#ifndef _MY_MALLOC_H
#define _MY_MALLOC_H

#define malloc(x) mymalloc(x)
#define free(x) myfree(x)

static char myblock[4096];

void *mymalloc(int x);
void free(int x);


#endif