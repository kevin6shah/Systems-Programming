#include <stdio.h>
#include <stdlib.h>
#include "huffman.h"


void huffcoder(treeNode* root, char *code, int index, char* book[], int *bookind){
    
    if (root->right != NULL){
        code[index] = '0';
        huffcoder(root->right, code, index + 1, book, bookind);
    }
    if (root->left != NULL){
        code[index] = '1';
        huffcoder(root->left, code, index + 1, book,bookind);
    }
    if (root->left == NULL && root->right == NULL){
        int n = strlen(root->token) + 1;
        book[*bookind] = malloc (n + index + 1);
        strcpy(book[*bookind], code);
        strcat(book[*bookind], ":");
        strcat(book[*bookind], root->token);
        ++(*bookind);
    }
}



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
    
    
    int bookind = 0;
    char **book = malloc(6 * sizeof(char*)); //replace 6 with total tokens inside
    char *code = malloc(10); //replace 10 with depth of huffmanTree
    huffcoder(treeHeap[0], code, 0, book, &bookind);
    int ind = 0;
    printf("\n");
    while (ind < 6){ //I put 6 here, replace with total tokens inside
        
        printf("%s\n", book[ind]);
        ind++;
    }
    
    
    return 0;
}

