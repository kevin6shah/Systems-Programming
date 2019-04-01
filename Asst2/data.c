#include "data.h"

void initializeTreeHeap(int size) {
	huffmanSize = 0;
	huffmanCapacity = size;
	treeHeap = malloc(sizeof(treeNode*) * huffmanCapacity);
}

int getLeftChildIndex(int parentIndex) { return (2 * parentIndex + 1); }
int getRightChildIndex(int parentIndex) { return (2 * parentIndex + 2); }
int getParentIndex(int childIndex) { return (childIndex - 1) / 2; }

int hasParent(int index) {
	if (getParentIndex(index) >= 0) return 1;
	return 0;
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
		printf("%s %d | ", node->token, node->frequency);
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
void nodeInsert(hashnode* node, hashnode **table){
    int key = getkey(node->token);
    if(table[key] == NULL){
        table[key] = node;
        return;
    }
    hashnode* ptr = table[key];
    hashnode* prev = table[key];
    while (ptr != NULL){
        prev = ptr;
        ptr = ptr->next;
    }
    prev->next = node;
    return;
    
    
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
		if (token == NULL) return -1;
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

char* getBit(char* token, hashnode **table){
    int key = getkey(token);
    hashnode *ptr = table[key];
    while (ptr != NULL){
        if (strcmp(ptr->token, token)==0){
            return ptr->bitcode;
        }
        ptr = ptr->next;
    }
    return NULL;
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
