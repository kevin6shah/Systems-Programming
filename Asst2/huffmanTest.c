#include <stdio.h>
#include <stdlib.h>
#include "huffman.h"

int main(int argc, char** argv) {
	
	initializeTreeHeap(6);
	treeNode* node = malloc(sizeof(treeNode));
	node->frequency = 5;
	node->token = "1";
	addTreeNode(node);
	treeNode* node1 = malloc(sizeof(treeNode));
	node1->frequency = 7;
	node1->token = "2";
	addTreeNode(node1);
	treeNode* node2 = malloc(sizeof(treeNode));
	node2->frequency = 10;
	node2->token = "3";
	addTreeNode(node2);
	treeNode* node3 = malloc(sizeof(treeNode));
	node3->frequency = 15;
	node3->token = "4";
	addTreeNode(node3);
	treeNode* node4 = malloc(sizeof(treeNode));
	node4->frequency = 20;
	node4->token = "5";
	addTreeNode(node4);
	treeNode* node5 = malloc(sizeof(treeNode));
	node5->frequency = 45;
	node5->token = "6";
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