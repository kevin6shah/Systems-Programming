#include "data.h"
#include "compressor.h"

treeNode* maketreeNode(char *token){
    treeNode *temp = malloc (sizeof(treeNode));
    int len = strlen(token) + 1;
    temp->token = malloc(len);
    strcpy(temp->token,token);
    temp->left = NULL;
    temp->right = NULL;
}

void addPath(char* bitcode, char* token, treeNode *root){
    int i = 0;
    treeNode *ptr = root;
    while (i < strlen(bitcode)){
        int dir;
        if (bitcode[i] == '0'){
            if (ptr->left == NULL){
                treeNode *temp = maketreeNode("NULL");
                ptr->left = temp;
            }
            ptr = ptr->left;
        } else {
            if (ptr->right == NULL){
                treeNode *temp = maketreeNode("NULL");
                ptr->right = temp;
            }
            ptr = ptr->right;
        }
        i++;
        
    }
    free(ptr->token);
    int len = strlen(token) + 1;
    ptr->token = malloc(len);
    strcpy(ptr->token,token);
    
}


void decompress(char* pathFile, char* pathHuffbook){
    hashnode **table = createTable();
    int len = bufferSize(pathFile) + 1;
    if (len == 0) return;
    char* file = malloc(len);//file holds the compressed bits
    strcpy(file, findBuffer(pathFile));
    len = 0;
    len = bufferSize(pathHuffbook) + 1;
    if (len == 0) return;
    char* buffer = malloc(len); //buffer holds huffbook
    
    strcpy(buffer, findBuffer(pathHuffbook));
    treeNode *root = malloc(sizeof(treeNode));
    root->token = "root";
    
    //build a qausi tree out of huffman
    int startIndex = 0;
    int endIndex = 0;
    int n = 0;
    int num = strlen(buffer);
    
    while(endIndex < num) { //insert buffer size
        while ((int)buffer[endIndex] != 9){
            endIndex++;
        }
        char *bitcode = malloc (((endIndex-startIndex)+2));
        strncpy(bitcode, buffer + startIndex, endIndex - startIndex);
        startIndex = ++endIndex;
        while ((int)buffer[endIndex] != 10){
            endIndex++;
        }
        char *token = malloc (((endIndex-startIndex)+2));
        strncpy(token, buffer + startIndex, endIndex - startIndex);
        printf("%s\n", token);
        if (strcmp("<\\t>", token)==0){
            token = "\t";
        }
        if (strcmp("<\\s>", token)==0){
            token = " ";
        }
        if (strcmp("<\\n>", token)==0){
            token = "\n";
        }
        if (strcmp("<\\v>", token)==0){
            token = "\v";
        }
        addPath(bitcode, token, root);
        
        startIndex = ++endIndex;
        
    }
    
    char *writePath = malloc (strlen(pathFile) -4);
    strncpy(writePath, pathFile, strlen(pathFile)-4);
    writePath[strlen(pathFile)-5] = '\0';
    
    int fd = open(writePath, O_WRONLY|O_CREAT, 0700);
    if (fd < 0) {
        printf("Error could not create the file!\n");
        return;
    }
    
    int i = 0;
    while (i < strlen(file)-1){
        treeNode *ptr = root;
        while (ptr->left != NULL || ptr->right != NULL){
            if (file[i] == '0'){
                ptr = ptr->left;
                i++;
            } else {
                ptr = ptr->right;
                i++;
            }
        }
        write(fd, ptr->token, strlen(ptr->token));
        ptr = root;
    }
    close(fd);
    
}

int main(int argc, char *argv[]){
    decompress(argv[1], argv[2]);
}
