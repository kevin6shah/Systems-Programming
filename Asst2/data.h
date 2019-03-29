#ifndef DATA
#define DATA
#define TABLESIZE 5000
#include "compressor.h"

typedef struct HashNode {
    int freq;
    char *token;
    struct HashNode *next;
} hashnode;

int getkey(char *token);
hashnode** createTable();
hashnode* createNode(char* token);
void hashInsert(char *token, hashnode** table);

#endif
