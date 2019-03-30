#include <stdio.h>
#include <stdlib.h>
#include "heap.h"

typedef struct treeNode {
	int frequency;
	char* token;
	struct treeNode* left;
	struct treeNode* right;
} treeNode;

treeNode** treeHeap;
int huffmanSize;
int huffmanCapacity;

void initializeTreeHeap() {
	huffmanSize = size;
	huffmanCapacity = size;
	treeHeap = malloc(sizeof(treeNode*) * huffmanCapacity);
	int i;
	for (i = 0; i < huffmanCapacity; i++) {
		treeNode *temp = malloc(sizeof(treeNode));
		temp->frequency = heap[i].frequency;
		temp->token = heap[i].token;
		temp->left = NULL;
		temp->right = NULL;
		treeHeap[i] = temp;
	}
}

int leftChildTreeHeap(int index) { return treeHeap[getLeftChildIndex(index)]->frequency; }
int rightChildTreeHeap(int index) { return treeHeap[getRightChildIndex(index)]->frequency; }
int parentTreeHeap(int index) { return treeHeap[getParentIndex(index)]->frequency; }

void swapTreeNode(int indexOne, int indexTwo) {
	treeNode* temp = treeHeap[indexOne];
	treeHeap[indexOne] = treeHeap[indexTwo];
	treeHeap[indexTwo] = temp;
}

treeNode* peekTreeNode() {
	if (huffmanSize == 0) return NULL;
	return treeHeap[0];
}

int hasLeftChildTree(int index) {
	if (getLeftChildIndex(index) < huffmanSize) {
		return 1;
	}
	return 0;
}

int hasRightChildTree(int index) {
	if (getRightChildIndex(index) < huffmanSize) return 1;
	return 0;
}

void traverse(treeNode* node) {
	if (node != NULL) {
		traverse(node->left);
		printf("%s %d ", node->token, node->frequency);
		traverse(node->right);
	}
}

void heapifyDownHuffman() {
	int index = 0;
	while (hasLeftChildTree(index)) {
		int smallestIndex = getLeftChildIndex(index);
		if (hasRightChildTree(index) && rightChildTreeHeap(index) < leftChildTreeHeap(index)) {
			smallestIndex = getRightChildIndex(index);
		}
		if (treeHeap[index]->frequency < treeHeap[smallestIndex]->frequency) {
			break;
		} else {
			swapTreeNode(index, smallestIndex);
		}
		index = smallestIndex;
	}
}

treeNode* pullTreeNode() {
	if (huffmanSize == 0) return NULL;
	treeNode *temp = treeHeap[0];
	treeHeap[0] = treeHeap[huffmanSize-1];
	huffmanSize--;
	heapifyDownHuffman();
	return temp;
}

void heapifyUpHuffman() {
	int index = huffmanSize - 1;
	while (hasParent(index) && parentTreeHeap(index) > treeHeap[index]->frequency) {
		swapTreeNode(getParentIndex(index), index);
		index = getParentIndex(index);
	}
}

void addTreeNode(treeNode *item) {
	treeHeap[huffmanSize] = item;
	huffmanSize++;
	heapifyUpHuffman();
}

void merge() {
	treeNode *smallest = pullTreeNode();
	treeNode *small = pullTreeNode();
	treeNode *merger = malloc(sizeof(treeNode));
	merger->frequency = smallest->frequency + small->frequency;
	merger->token = "***";
	merger->left = smallest;
	merger->right = small;
	addTreeNode(merger);
}

// TRYING TO FIGURE OUT HOW TO GET TO PRINT THE PATH FROM THE TREE

void bitCode(treeNode* node, int bit, char direction) {
	/*
	if (node == NULL) return;
	if (direction == 'L') printf("0");
	else if (direction == 'R') printf("1");
	if (strcmp(node->token, "***") != 0) {
		printf("\t%s\n", node->token);
	}
	bit<<=1;
	bitCode(node->left, bit, 'L');
	bitCode(node->right, bit+1, 'R');
	*/
	
	
}

int main(int argc, char** argv) {
	initializeHeap(15);
	heapNode* node = malloc(sizeof(heapNode));
	node->frequency = 45;
	node->token = "A";
	add(node);
	heapNode* node1 = malloc(sizeof(heapNode));
	node1->frequency = 4;
	node1->token = "B";
	add(node1);
	heapNode* node2 = malloc(sizeof(heapNode));
	node2->frequency = 67;
	node2->token = "C";
	add(node2);
	heapNode* node3 = malloc(sizeof(heapNode));
	node3->frequency = 54;
	node3->token = "D";
	add(node3);
	heapNode* node4 = malloc(sizeof(heapNode));
	node4->frequency = 2;
	node4->token = "E";
	add(node4);
	heapNode* node5 = malloc(sizeof(heapNode));
	node5->frequency = 23;
	node5->token = "F";
	add(node5);
	
	initializeTreeHeap();
	while (huffmanSize != 1) {
		merge();
	}
	// TRYING TO FIGURE OUT HOW TO GET TO PRINT THE PATH FROM THE TREE
	bitCode(treeHeap[0], 0, '*');
	return 0;
}