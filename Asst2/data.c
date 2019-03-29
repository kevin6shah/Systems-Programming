#include "data.h"

//first create struct HashTable

#define TABLESIZE 200

typedef struct HashNode {
    
    int freq;
    char *token;
    int key;
    struct HasNode *next;
}Node;

node *HashTable[TABLESIZE];

//takes the token, and adds the acsii values of each char and hashes it with table size.
int hashcode(char *token){
    
}
