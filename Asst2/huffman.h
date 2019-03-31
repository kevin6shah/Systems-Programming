#ifndef HUFFMAN_HEAP
#define HUFFMAN_HEAP

#include "compressor.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct treeNode {
	int frequency;
	char* token;
	struct treeNode* left;
	struct treeNode* right;
} treeNode;

treeNode** treeHeap;
int huffmanSize;
int huffmanCapacity;

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
void bitCode(treeNode* node, int bit, char direction);

#endif
