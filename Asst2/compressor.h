#ifndef FILE_COMPRESSOR
#define FILE_COMPRESSOR

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "data.h"

char* findBuffer(char* str);
int bufferSize(char* str);
void huffcoder(treeNode* root, char *code, int index, char* book[], int *bookind);
int findHeight(treeNode* root);
void makeCodeBook(char* path, char** book);
int spacecheck(char c);
int tokenize(hashnode **HASH, char *buffer, int len);
void makeHeap(hashnode **table);
void build(char* filePath, char* huffmanPath);
void buildRecursiveHelper(char* filePath, hashnode **HASHTABLE, int *cap);
void buildRecursive(char* filePath, char* huffmanPath);
hashnode** compressInit(char* pathFile, char* pathHuffbook);
void compress(char* pathFile, hashnode** table);
void compressRecursive(char* filePath, hashnode** table);
int isDir(char* path);
int isHuffman(char *path);
treeNode* maketreeNode(char *token);
void addPath(char* bitcode, char* token, treeNode *root);
treeNode* decompressInit(char* pathFile, char* pathHuffbook);
void decompress(char* pathFile, treeNode *root);
void decompressRecursive(char* filePath, treeNode* root);
void printStatements();

#endif
