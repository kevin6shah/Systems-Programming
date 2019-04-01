#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "data.h"

char* findBuffer(char* str) {
    int fd = open(str, O_RDONLY);
    if (fd == -1) {
        printf("ERROR\n");
        return NULL;
    }
    char temp;
    int result, counter;
    while ((result = read(fd, &temp, 1)) != 0) {
        counter++;
    }
    close(fd);
    fd = open(str, O_RDONLY);
    char *buffer = malloc(++counter);
    result = read(fd, buffer, counter);
    if (result == -1) {
        printf("ERROR 2\n");
        return NULL;
    }
    buffer[result] = '\0';
    return buffer;
    
}

void compress(char* pathHuffbook, char* pathFile, hashnode** table){
    char* file = malloc(2000);
    strcpy(file, findBuffer(pathFile));
    char* buffer = malloc (2000);
    strcpy(buffer, findBuffer(pathHuffbook));
    
    //first store the huffman codebook inside hashtable
    int startIndex = 2;
    int endIndex = 2;
    int n = 0;
    while((int)buffer[endIndex] != 10){ //insert buffer size
        while ((int)buffer[endIndex] != 9){
            endIndex++;
        }
        char *bitcode = malloc (((endIndex-startIndex)+2)*sizeof(char));
        strncpy(bitcode, buffer + startIndex, endIndex - startIndex);
        startIndex = ++endIndex;
        while ((int)buffer[endIndex] != 10){
            endIndex++;
        }
        char *token = malloc (((endIndex-startIndex)+2)*sizeof(char));
        strncpy(token, buffer + startIndex, endIndex - startIndex);
        if (strcmp("<\\t>", token)==0){
            token = "\t";
        }
        if (strcmp("<\\s>", token)==0){
            token = " ";
        }
        if (strcmp("<\\n>", token)==0){
            token = "\n";
        }
        if (strcmp("<\\v>", token)==0){
            token = "\v";
        }
        hashnode* temp = createNode(token);
        temp->bitcode = bitcode;
        
        nodeInsert(temp, table);
        n++;
        
        startIndex = ++endIndex;
        
    }
    printHash(table);
    
    
}




int main(int argc, char* argv[]){
    char *path = malloc (1000);
    hashnode** table = createTable();
    compress(argv[1],argv[2], table);
}

