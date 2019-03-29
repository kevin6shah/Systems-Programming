#include "data.h"

//first create struct HashTable

#define TABLESIZE 5000

typedef struct HashNode {
    
    int freq;
    char *token;
    struct HashNode *next;
}hashnode;


hashnode** createTable(){
    hashnode** HashTable = malloc (TABLESIZE * sizeof(hashnode*));
    return HashTable;
}

hashnode* createNode(char* token){
    hashnode *temp = malloc (sizeof(struct HashNode));
    int len = strlen(token) + 1;
    temp->token = malloc (len *sizeof(char));
    strcpy(temp->token, token);
    temp->freq = 1;
    temp->next = NULL;
    return temp;
}

int getkey(char *token){
    int len = strlen(token);
    int i = 0;
    int sum = 0;
    while(i < len){
        sum += (int)token[i];
        i++;
    }
    int key = sum % TABLESIZE;
    return key;
}

void hashInsert(char *token, hashnode** table){
    int key = getkey(token);
    if (table[key] == NULL){
        table[key] = createNode(token);
        return;
    }
    hashnode* ptr = table[key];
    hashnode* prev = table[key];
    while (ptr != NULL){
        if (strcmp(ptr->token, token) == 0){
            ++ptr->freq;
            return;
        }
        prev = ptr;
        ptr = ptr->next;
    }
    hashnode* temp = createNode(token);
    prev->next = temp;
    return;
}


int main(int argc, char *argv[]){
    hashnode **HashTable = createTable();
    char string[] = "a";
    char s[] = "a";
    hashInsert(string, HashTable);
    hashInsert(s, HashTable);
    printf("%d\n", HashTable[97]->freq);
}
