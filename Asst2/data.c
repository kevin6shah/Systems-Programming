#include "data.h"

//first create struct HashTable


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

int hashInsert(char *token, hashnode** table){
    int key = getkey(token);
    if (table[key] == NULL){
        table[key] = createNode(token);
        return 1;
    }
    hashnode* ptr = table[key];
    hashnode* prev = table[key];
    while (ptr != NULL){
        if (strcmp(ptr->token, token) == 0){
            ++ptr->freq;
            return 0;
        }
        prev = ptr;
        ptr = ptr->next;
    }
    hashnode* temp = createNode(token);
    prev->next = temp;
    return 1;
}

void printHash(hashnode** table){
    int i = 0;
    while (i < TABLESIZE){
        if (table[i] != NULL){
            hashnode *ptr = table[i];
            while(ptr!= NULL){
                if ((int)ptr->token[0] == 32){
                    printf("Token: space Freq: %d\n",  ptr->freq);
                    ptr=ptr->next;
                    continue;
                }
                if ((int)ptr->token[0] == 9){
                    printf("Token: tab Freq: %d\n",  ptr->freq);
                    ptr=ptr->next;
                    continue;
                }
                if ((int)ptr->token[0] == 10){
                    printf("Token: NL Freq: %d\n",  ptr->freq);
                    ptr=ptr->next;
                    continue;
                }
                
                printf("Token: %s Freq: %d\n", ptr->token, ptr->freq);
                ptr = ptr->next;
            }
        }
        i++;
    }
    ////printf("tab?: %d", table[9]->freq);
}
