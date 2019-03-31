#include <stdio.h>
#include <stdlib.h>
#include "huffman.h"

int main(int argc, char** argv) {
	
	initializeTreeHeap(6);
	treeNode* node = malloc(sizeof(treeNode));
	node->frequency = 45;
	node->token = "A";
	addTreeNode(node);
	treeNode* node1 = malloc(sizeof(treeNode));
	node1->frequency = 4;
	node1->token = "B";
	addTreeNode(node1);
	treeNode* node2 = malloc(sizeof(treeNode));
	node2->frequency = 67;
	node2->token = "C";
	addTreeNode(node2);
	treeNode* node3 = malloc(sizeof(treeNode));
	node3->frequency = 54;
	node3->token = "D";
	addTreeNode(node3);
	treeNode* node4 = malloc(sizeof(treeNode));
	node4->frequency = 2;
	node4->token = "E";
	addTreeNode(node4);
	treeNode* node5 = malloc(sizeof(treeNode));
	node5->frequency = 23;
	node5->token = "F";
	addTreeNode(node5);

	/* JUST TO SEE IF IT PULLS IN MIN ORDER
	while (peekTreeNode() != NULL) {
		treeNode* temp = pullTreeNode();
		printf("%s %d | ", temp->token, temp->frequency);
	}*/
	
	
	while (huffmanSize != 1) {
		merge();
	}
	traverse(treeHeap[0]);
	
	// TRYING TO FIGURE OUT HOW TO GET TO PRINT THE PATH FROM THE TREE
	//bitCode(treeHeap[0], 0, '*');
	// ^^^^^^^^^THIS FUNCTION^^^^^^^^^
	
	return 0;
}