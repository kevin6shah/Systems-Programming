#include "heap.h"

void initializeHeap(int num) {
	capacity = num;
	size = 0;
	heap = malloc(sizeof(heapNode)*capacity);
}

int getLeftChildIndex(int parentIndex) { return (2 * parentIndex + 1); }

int getRightChildIndex(int parentIndex) { return (2 * parentIndex + 2); }

int getParentIndex(int childIndex) { return (childIndex - 1) / 2; }

int hasLeftChild(int index) {
	if (getLeftChildIndex(index) < size) {
		return 1;
	}
	return 0;
}

int hasRightChild(int index) {
	if (getRightChildIndex(index) < size) return 1;
	return 0;
}

int hasParent(int index) {
	if (getParentIndex(index) >= 0) return 1;
	return 0;
}

int leftChild(int index) { return heap[getLeftChildIndex(index)].frequency; }

int rightChild(int index) { return heap[getRightChildIndex(index)].frequency; }

int parent(int index) { return heap[getParentIndex(index)].frequency; }

void swap(int indexOne, int indexTwo) {
	heapNode temp = heap[indexOne];
	heap[indexOne] = heap[indexTwo];
	heap[indexTwo] = temp;
}

void ensureExtraCapacity() {
	if (size == capacity) {
		heap = realloc(heap, sizeof(heapNode)*capacity*2);
		capacity*=2;
	}
}

heapNode* peek() {
	if (size == 0) return NULL;
	return &heap[0];
}

heapNode* pull() {
	if (size == 0) return NULL;
	heapNode temp = heap[0];
	heapNode *item = &temp;
	heap[0] = heap[size-1];
	size--;
	heapifyDown();
	return item;
}

void add(heapNode *item) {
	ensureExtraCapacity();
	heap[size] = *item;
	size++;
	heapifyUp();
}

void heapifyUp() {
	int index = size - 1;
	while (hasParent(index) && parent(index) > heap[index].frequency) {
		swap(getParentIndex(index), index);
		index = getParentIndex(index);
	}
}

void heapifyDown() {
	int index = 0;
	while (hasLeftChild(index)) {
		int smallestIndex = getLeftChildIndex(index);
		if (hasRightChild(index) && rightChild(index) < leftChild(index)) {
			smallestIndex = getRightChildIndex(index);
		}
		if (heap[index].frequency < heap[smallestIndex].frequency) {
			break;
		} else {
			swap(index, smallestIndex);
		}
		index = smallestIndex;
	}
}