#ifndef DATA
#define DATA
#define TABLESIZE 5000

#include <stdio.h>
#include <stdlib.h>

#include <string.h>


typedef struct HashNode {
    int freq;
    char *token;
    struct HashNode *next;
} hashnode;

int getkey(char *token);
hashnode** createTable();
hashnode* createNode(char* token);
int hashInsert(char *token, hashnode** table);
void printHash(hashnode** table);

#endif
