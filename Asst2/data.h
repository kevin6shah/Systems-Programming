#ifndef DATA
#define DATA
#define TABLESIZE 5000

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct HashNode {
    int freq;
    char *token;
    char *bitcode;
    struct HashNode *next;
} hashnode;

typedef struct treeNode {
	int frequency;
	char* token;
	struct treeNode* left;
	struct treeNode* right;
} treeNode;

treeNode** treeHeap;
int huffmanSize;
int huffmanCapacity;

int getkey(char *token);
hashnode** createTable();
hashnode* createNode(char* token);
int hashInsert(char *token, hashnode** table);
void printHash(hashnode** table);
void initializeTreeHeap(int size);
int getLeftChildIndex(int parentIndex);
int getRightChildIndex(int parentIndex);
int getParentIndex(int childIndex);
int hasParent(int index);
int leftChildTreeHeap(int index);
int rightChildTreeHeap(int index);
int parentTreeHeap(int index);
void swapTreeNode(int indexOne, int indexTwo);
treeNode* peekTreeNode();
int hasLeftChildTree(int index);
int hasRightChildTree(int index);
void traverse(treeNode* node);
void heapifyDownHuffman();
treeNode* pullTreeNode();
void heapifyUpHuffman();
void addTreeNode(treeNode *item);
void merge();

#endif
