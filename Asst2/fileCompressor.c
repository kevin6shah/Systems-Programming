#include "compressor.h"
#include "data.h"

void huffcoder(treeNode* root, char *code, int index, char* book[], int *bookind) {
    if (root->left != NULL){
        code[index] = '0';
        huffcoder(root->left, code, index + 1, book,bookind);
    }

    if (root->right != NULL){
        code[index] = '1';
        huffcoder(root->right, code, index + 1, book, bookind);
    }

    if (root->left == NULL && root->right == NULL){
        int n = strlen(root->token) + 1;
        book[*bookind] = malloc (n + index + 1);
        strncpy(book[*bookind], code, index);
        char* tab = malloc (2);
        tab[0] = '\t';
        tab[1] = '\0';
        strcat(book[*bookind], tab);
        strcat(book[*bookind], root->token);
        ++(*bookind);
    }
}

int findHeight(treeNode* root) {
  if (root == NULL) {
    return 0;
  }
  else {
    int leftDepth = findHeight(root->left);
    int rightDepth = findHeight(root->right);
    if (rightDepth > leftDepth) return rightDepth+1;
    else return leftDepth+1;
  }
}

void makeCodeBook(char* path, char** book) {
  int fd = open(path, O_RDONLY);
  if (fd < 0) {
    printf("Error: %s is not a valid path!\n", path);
    return;
  }
  close(fd);

  path = realloc(path, strlen(path)+16);
  strcat(path, "HuffmanCodebook");
  fd = open(path, O_WRONLY|O_CREAT, 0700);
  if (fd < 0) {
    printf("Error could not create the file!\n");
    return;
  }
  
  write(fd, "\\\n", 3);
  int i;
  for (i = 0; i < huffmanCapacity; i++) {
    char *newLine = "\n";
    book[i] = realloc(book[i], strlen(book[i]) + 2);
    strcat(book[i], newLine);
    write(fd, book[i] , strlen(book[i]));
  }
}

//returns 0 if a space character
//returns 1 if not
int spacecheck(char c){
    if ((int) c >= 0 && (int) c < 33) return 0;
    return 1;
}

//turn buffer string, and input tokens into the HashTable
int tokenize(hashnode **HASH, char *buffer, int len){
    int total = 0;
    int i = 0;
    int startIndex = 0;
    int endIndex = 0;
    int notspace = 0;
    while (i < (len-1)){
        if (spacecheck(buffer[i])==1){
            startIndex = i;
            endIndex = i;
            while(spacecheck(buffer[endIndex]) != 0 && endIndex < (len-1)){
                endIndex++;
            }
            char *temp = malloc (((endIndex-startIndex)+2)*sizeof(char));
            strncpy(temp, buffer + startIndex, endIndex - startIndex);
            total += hashInsert(temp,HASH);
            i = endIndex;
            continue;
        }
        if (spacecheck(buffer[i])==0){
            char *temp = malloc (2*sizeof(char));
            strncpy(temp, buffer + i, 1);
            temp[1] = '\0';
            total += hashInsert(temp,HASH);
            i++;
            continue;
        }

    }
    return total;

}

void makeHeap(hashnode **table){
    int i = 0;
    while (i < TABLESIZE){
        if (table[i] != NULL){
            hashnode *ptr = table[i];
            while(ptr!= NULL){
                treeNode *temp = malloc(sizeof(treeNode));
                temp->frequency = ptr->freq;
                int len = strlen(ptr->token) + 1;
                temp->token = malloc(len * sizeof(char));
                strcpy(temp->token, ptr->token);
                addTreeNode(temp);
                ptr = ptr->next;
            }
        }
        i++;
    }
}

void build(char* filePath, char* huffmanPath) {
  int len = bufferSize(filePath) + 1;
  if (len == 0) return;
  char *buffer = malloc(len);
  strcpy(buffer, findBuffer(filePath));
  hashnode **HASHTABLE = createTable();
  int cap = tokenize(HASHTABLE, buffer, len);

  initializeTreeHeap(cap);
  makeHeap(HASHTABLE);
  while (huffmanSize != 1) {
    merge();
  }

  int bookind = 0;
  char **book = malloc(huffmanCapacity * sizeof(char*));
  char *code = malloc(findHeight(treeHeap[0]));
  huffcoder(treeHeap[0], code, 0, book, &bookind);

  char* path = malloc(strlen(huffmanPath));
  strcpy(path, huffmanPath);
  makeCodeBook(path, book);
}

int main(int argc, char* argv[]) {
    if (!(argc > 3 && argc < 6)) {
      printf("Invalid arguments!\nUsage: ./fileCompressor <flag> <path or file> |codebook|\n");
      return 0;
    }

    //short recursive = 0, build = 0, compress = 0, decompress = 0;
    if (argc == 5) {
      if (strcmp(argv[1], "-R") == 0 && strcmp(argv[2], "-b") == 0) {
        //recursive = 1;
        //build = 1;
      } else if (strcmp(argv[1], "-R") == 0 && strcmp(argv[2], "-c") == 0) {
        //recursive = 1;
        //compress = 1;
      } else if (strcmp(argv[1], "-R") == 0 && strcmp(argv[2], "-d") == 0) {
        //recursive = 1;
        //decompress = 1;
      } else {
        printf("Invalid arguments!\nUsage: ./fileCompressor <flag> <path or file> |codebook|\n");
        return 0;
      }
    }

    if (argc == 4) {
      if (strcmp(argv[1], "-b") == 0) build(argv[2], argv[3]);
      else if (strcmp(argv[1], "-c") == 0) /*compress = 1*/;
      else if (strcmp(argv[1], "-d") == 0) /*decompress = 1*/;
      else {
        printf("Invalid arguments!\nUsage: ./fileCompressor <flag> <path or file> |codebook|\n");
        return 0;
      }
    }
    return 0;
}
