#include "compressor.h"
#include "data.h"
#include "heap.h"

//returns 0 if a space character
//returns 1 if not
int spacecheck(char c){
    if ((int) c >= 0 && (int) c < 33) return 0;
    return 1;
}

//turn buffer string, and input tokens into the HashTable
int store(hashnode **HASH, char *buffer, int len){
    int total = 0;
    int i = 0;
    int startIndex = 0;
    int endIndex = 0;
    int notspace = 0;
    while (i < (len-1)){
        if (spacecheck(buffer[i])==1){
            startIndex = i;
            endIndex = i;
            while(spacecheck(buffer[endIndex]) != 0 && endIndex < (len-1)){
                endIndex++;
            }
            char *temp = malloc (((endIndex-startIndex)+2)*sizeof(char));
            strncpy(temp, buffer + startIndex, endIndex - startIndex);
            total += hashInsert(temp,HASH);
            i = endIndex;
            continue;
        }
        if (spacecheck(buffer[i])==0){
            char *temp = malloc (2*sizeof(char));
            strncpy(temp, buffer + i, 1);
            temp[1] = '\0';
            total += hashInsert(temp,HASH);
            i++;
            continue;
        }
        
    }
    return total;
    
}

void makeHeap(hashnode **table){
    int i = 0;
    while (i < TABLESIZE){
        if (table[i] != NULL){
            hashnode *ptr = table[i];
            while(ptr!= NULL){
                heapNode *temp = malloc (sizeof(heapNode));
                temp->frequency = ptr->freq;
                int len = strlen(ptr->token) + 1;
                temp->token = malloc (len *sizeof(char));
                strcpy(temp->token, ptr->token);
                add(temp);
                ptr = ptr->next;
                
            }
        }
        i++;
    }
}






int main(int argc, char* argv[]){
    int len = strlen(tokenize(argv[1])) + 1;
    char *buffer = malloc(len*sizeof(char));
    strcpy(buffer, tokenize(argv[1]));
    hashnode **HASHTABLE = createTable();
    int cap = store(HASHTABLE, buffer, len);
    //printHash(HASHTABLE);
    initializeHeap(cap);
    printHeap();
    
    
}
