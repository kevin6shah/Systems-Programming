#ifndef _HEAP
#define _HEAP

#include "compressor.h"

#include <stdio.h>
#include <stdlib.h>

#include <string.h>


typedef struct {
	int frequency;
	char* token;
} heapNode;

int capacity;
int size;
heapNode *heap;

void initializeHeap(int num);
int getLeftChildIndex(int parentIndex);
int getRightChildIndex(int parentIndex);
int getParentIndex(int childIndex);
int hasLeftChild(int index);
int hasRightChild(int index);
int hasParent(int index);
int leftChild(int index);
int rightChild(int index);
int parent(int index);
void swap(int indexOne, int indexTwo);
void ensureExtraCapacity();
heapNode* peek();
heapNode* pull();
void add(heapNode* item);
void heapifyUp();
void heapifyDown();
void printHeap();

#endif
