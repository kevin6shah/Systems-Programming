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
void store(hashnode **HASH, char *buffer, int len){
    int i;
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
            hashInsert(temp,HASH);
            //printf("%s\n", temp);
            i = endIndex;
            continue;
        }
        if (spacecheck(buffer[i])==0){
            char *temp = malloc (2*sizeof(char));
            strncpy(temp, buffer + i, 1);
            //printf("%d\n",(int)buffer[i]);
            hashInsert(temp,HASH);
            i++;
            continue;
        }
    }
    
}






int main(int argc, char* argv[]){
    int len = strlen(tokenize(argv[1])) + 1;
    char *buffer = malloc(len*sizeof(char));
    strcpy(buffer, tokenize(argv[1]));
    hashnode **HASHTABLE = createTable();
    store(HASHTABLE, buffer, len);
    printHash(HASHTABLE);
    
    
}
