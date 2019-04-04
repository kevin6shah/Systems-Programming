#include "compressor.h"
#include "data.h"
#include <unistd.h>

int bufferSize(char* str) {
	int fd = open(str, O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "Could not open the file: %s\n", str);
		return -1;
	}
	char temp;
	int counter = 0;
	while (read(fd, &temp, 1) != 0) {
		counter++;
	}
	close(fd);
	return counter;
}

char* findBuffer(char* str) {
	int bufSize = bufferSize(str);
	if (bufSize == -1) return NULL;
	int fd = open(str, O_RDONLY);
	char *buffer = malloc(++bufSize);
	int result = read(fd, buffer, bufSize);
	if (result < 0) {
		fprintf(stderr, "Could not read the file: %s\n", str);
		return NULL;
	}
	close(fd);
	buffer[result] = '\0';
  return buffer;
}

void huffcoder(treeNode* root, char *code, int index, char* book[], int *bookind) {
    if (root->left != NULL){
        code[index] = '0';
        huffcoder(root->left, code, index + 1, book,bookind);
    }

    if (root->right != NULL) {
        code[index] = '1';
        huffcoder(root->right, code, index + 1, book, bookind);
    }

    if (root->left == NULL && root->right == NULL) {
        char *token = NULL;
        if (strcmp(root->token, "\t") == 0) {
          token = malloc(5);
          strcpy(token, "<\\t>");
        } else if (strcmp(root->token, " ") == 0) {
          token = malloc(5);
          strcpy(token, "<\\s>");
        } else if (strcmp(root->token, "\v") == 0) {
          token = malloc(5);
          strcpy(token, "<\\v>");
        } else if (strcmp(root->token, "\n") == 0) {
          token = malloc(5);
          strcpy(token, "<\\n>");
        }
        int n;
        if (token == NULL) n = strlen(root->token) + 1;
        else n = 5;
        book[*bookind] = malloc (n + index + 1);
        strncpy(book[*bookind], code, index);
        char* tab = malloc (2);
        tab[0] = '\t';
        tab[1] = '\0';
        strcat(book[*bookind], tab);
        if (token == NULL) strcat(book[*bookind], root->token);
        else strcat(book[*bookind], token);
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
    fprintf(stderr, "Error: %s is not a valid path!\n", path);
    return;
  }
  close(fd);

  path = realloc(path, strlen(path)+16);
  strcat(path, "HuffmanCodebook");
  fd = open(path, O_WRONLY|O_CREAT, 0700);
  if (fd < 0) {
    fprintf(stderr, "Error could not create the file!\n");
    return;
  }

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
  printf("%d\n", len);
  if (len == 0) return;
  if (len == 1) { // Edge Case: In case the file given is NULL
    char *path = malloc(strlen(huffmanPath)+16);
    strcpy(path, huffmanPath);
    strcat(path, "HuffmanCodebook");
    int fd = open(path, O_RDONLY | O_CREAT, 0700);
    fprintf(stderr, "Warning: Given file '%s' was empty.\nEmpty Codebook created\n", filePath);
    close(fd);
    return;
  }
  char *buffer = malloc(len);
  strcpy(buffer, findBuffer(filePath));

  hashnode **HASHTABLE = createTable();
  int cap = tokenize(HASHTABLE, buffer, len);
  initializeTreeHeap(cap);
  makeHeap(HASHTABLE);

  short oneToken = 0;
  if (huffmanSize == 1) oneToken = 1;
  while (huffmanSize > 1) {
    merge();
  }

  int bookind = 0;
  char **book = malloc(huffmanCapacity * sizeof(char*));
  char *code = malloc(findHeight(treeHeap[0]) + 1);
  if (oneToken == 0) huffcoder(treeHeap[0], code, 0, book, &bookind);
  else {
    char *temp = malloc(strlen("1\t") + strlen(treeHeap[0]->token) + 1);
    strcpy(temp, "1\t");
    strcat(temp, treeHeap[0]->token);
    book[0] = temp;
  }

  char* path = malloc(strlen(huffmanPath)+1);
  strcpy(path, huffmanPath);
  makeCodeBook(path, book);
}

void buildRecursiveHelper(char* filePath, hashnode **HASHTABLE, int *cap) {
  DIR *directory = opendir(filePath);
	struct dirent* temp;
	if (directory == NULL) {
		fprintf(stderr, "Could not find the directory!\n");
		return;
	}
  int i = 0;
  while ((temp = readdir(directory)) != NULL) {
    if (temp->d_type == 4 && strcmp(temp->d_name, ".") != 0 && strcmp(temp->d_name, "..") != 0) {
      char* buffer = malloc(2+temp->d_reclen+strlen(filePath));
			strcpy(buffer, filePath);
			strcat(buffer, "/");
			strcat(buffer, temp->d_name);
			buffer[strlen(buffer)] = '\0';
			buildRecursiveHelper(buffer, HASHTABLE, cap);
    }
    else {
      if (!(i < 2)) {
        int count = 3+strlen(filePath)+temp->d_reclen;
				char* buffer = malloc(count);
				strcpy(buffer, filePath);
				strcat(buffer, "/");
				strcat(buffer, temp->d_name);
				buffer[strlen(buffer)] = '\0';
        int len = bufferSize(buffer) + 1;
        if (len == 0) return;
        if (len != 1) {
          char *buffer2 = malloc(len);
          strcpy(buffer2, findBuffer(buffer));
          *cap = *cap + tokenize(HASHTABLE, buffer2, len);
        }
      }
    }
    i++;
  }
}

void buildRecursive(char* filePath, char* huffmanPath) {
  hashnode **HASHTABLE = createTable();
  int cap;
  buildRecursiveHelper(filePath, HASHTABLE, &cap);
  initializeTreeHeap(cap);
  makeHeap(HASHTABLE);
  while (huffmanSize != 1) {
    merge();
  }

  int bookind = 0;
  char **book = malloc(huffmanCapacity * sizeof(char*));
  char *code = malloc(findHeight(treeHeap[0]));
  huffcoder(treeHeap[0], code, 0, book, &bookind);

  char* path = malloc(strlen(huffmanPath)+1);
  strcpy(path, huffmanPath);
  makeCodeBook(path, book);
}

hashnode** compressInit(char* pathFile, char* pathHuffbook) {
    hashnode** table = createTable();
    int len = 0;
    len = bufferSize(pathHuffbook) + 1;
    if (len == 0 || len == 1) return NULL;
    char* buffer = malloc(len);

    strcpy(buffer, findBuffer(pathHuffbook));

    //first store the huffman codebook inside hashtable
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
        hashnode* temp = createNode(token);
        temp->bitcode = bitcode;

        nodeInsert(temp, table);
        n++;

        startIndex = ++endIndex;
    }
    return table;
  }

void compress(char* pathFile, hashnode** table) {
    int len = bufferSize(pathFile) + 1;
    if (len == 0) return;
    if (len == 1) {
      char *writePath = malloc (strlen(pathFile) + 5);
      strcpy(writePath,pathFile);
      strcat(writePath, ".hcz");
			printf("Path: %s\n", writePath);
      int fd = open(writePath, O_WRONLY|O_CREAT, 0700);
      fprintf(stderr, "Warning: Tried compressing an empty file!\nEmpty compressed file created!\n");
      return;
    }
    char* file = malloc(len);
    strcpy(file, findBuffer(pathFile));

    //now to actually compress into new file
    char *writePath = malloc (strlen(pathFile) + 5);
    strcpy(writePath,pathFile);
    strcat(writePath, ".hcz");

    int fd = open(writePath, O_WRONLY|O_CREAT, 0700);
    if (fd < 0) {
        fprintf(stderr, "Error could not create the file!\n");
        return;
    }
    int i = 0, startIndex = 0, endIndex = 0;
    while (i < strlen(file)) {
        if (spacecheck(file[i])==1){
            startIndex = i;
            endIndex = i;
            while(spacecheck(file[endIndex]) != 0 && endIndex < (strlen(file))){
                endIndex++;
            }
            char *temp = malloc (((endIndex-startIndex)+2)*sizeof(char));
            strncpy(temp, file + startIndex, endIndex - startIndex);
            write(fd, getBit(temp,table), strlen(getBit(temp,table)));
            i = endIndex;
            continue;
        }
        if (spacecheck(file[i])==0){
            char *temp = malloc (2*sizeof(char));
            strncpy(temp, file + i, 1);
            temp[1] = '\0';
            write(fd, getBit(temp,table), strlen(getBit(temp,table)));
            i++;
            continue;
        }
    }
    close(fd);
}

void compressRecursive(char* filePath, hashnode** table) {
  DIR *directory = opendir(filePath);
	struct dirent* temp;
	if (directory == NULL) {
		fprintf(stderr, "Could not find the directory!\n");
		return;
	}
  int i = 0;
  while ((temp = readdir(directory)) != NULL) {
    if (temp->d_type == 4 && strcmp(temp->d_name, ".") != 0 && strcmp(temp->d_name, "..") != 0) {
      char* buffer = malloc(2+temp->d_reclen+strlen(filePath));
			strcpy(buffer, filePath);
			strcat(buffer, "/");
			strcat(buffer, temp->d_name);
			buffer[strlen(buffer)] = '\0';
			compressRecursive(buffer, table);
    }
    else {
      if (!(i < 2)) {
        int count = 3+strlen(filePath)+temp->d_reclen;
				char* buffer = malloc(count);
				strcpy(buffer, filePath);
				strcat(buffer, "/");
				strcat(buffer, temp->d_name);
				buffer[strlen(buffer)] = '\0';
        compress(buffer, table);
      }
    }
    i++;
  }
}

int isDir(char* path) {
  DIR *directory = opendir(path);
  if (directory == NULL) {
    return 0;
  } else {
    return 1;
  }
}

int isHuffman(char *path) {
  int fd = open(path, O_RDONLY);
  if (fd < 0) return 0;
  if (strlen(path) < 15) return 0;
  if (strcmp(path, "HuffmanCodebook") == 0) return 1;
  if (strcmp(path+(strlen(path)-16), "/HuffmanCodebook") == 0) return 1;
  return 0;
}

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

treeNode* decompressInit(char* pathFile, char* pathHuffbook){
    int len = 0;
    len = bufferSize(pathHuffbook) + 1;
    if (len == 0 || len == 1) return NULL;
    char* buffer = malloc(len); //buffer holds huffbook

    strcpy(buffer, findBuffer(pathHuffbook));
    treeNode *root = malloc(sizeof(treeNode));
    root->token = "root";

    //build a qausi tree out of huffman
    int startIndex = 0;
    int endIndex = 0;
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
    return root;
}

void decompress(char* pathFile, treeNode *root) {
    int len = bufferSize(pathFile) + 1;
    if (len == 0) return;
    if (len == 1) {
      char *writePath = malloc (strlen(pathFile) -3);
      strncpy(writePath, pathFile, strlen(pathFile)-4);
      writePath[strlen(pathFile)-4] = '\0';
			printf("Path: %s\n", writePath);
      int fd = open(writePath, O_WRONLY|O_CREAT, 0700);
      fprintf(stderr, "Warning: Tried decompressing an empty file!\nEmpty decompressed file created!\n");
      return;
    }
    char* file = malloc(len);//file holds the compressed bits
    strcpy(file, findBuffer(pathFile));
    char *writePath = malloc (strlen(pathFile) -3);
    strncpy(writePath, pathFile, strlen(pathFile)-4);
    writePath[strlen(pathFile)-4] = '\0';

    int fd = open(writePath, O_WRONLY|O_CREAT, 0700);
    if (fd < 0) {
        fprintf(stderr, "Error could not create the file!\n");
        return;
    }

    int i = 0;
    while (i < strlen(file)){
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

void decompressRecursive(char* filePath, treeNode* root) {
  DIR *directory = opendir(filePath);
	struct dirent* temp;
	if (directory == NULL) {
		fprintf(stderr, "Could not find the directory!\n");
		return;
	}
  int i = 0;
  while ((temp = readdir(directory)) != NULL) {
    if (temp->d_type == 4 && strcmp(temp->d_name, ".") != 0 && strcmp(temp->d_name, "..") != 0) {
      char* buffer = malloc(2+temp->d_reclen+strlen(filePath));
			strcpy(buffer, filePath);
			strcat(buffer, "/");
			strcat(buffer, temp->d_name);
			buffer[strlen(buffer)] = '\0';
			decompressRecursive(buffer, root);
    }
    else {
      if ((!(i < 2)) && strstr(temp->d_name, ".hcz") != NULL && temp->d_name[strlen(temp->d_name)-1] == 'z') {
        int count = 3+strlen(filePath)+temp->d_reclen;
				char* buffer = malloc(count);
				strcpy(buffer, filePath);
				strcat(buffer, "/");
				strcat(buffer, temp->d_name);
				buffer[strlen(buffer)] = '\0';
        decompress(buffer, root);
      }
    }
    i++;
  }
}

void printStatements() {
  fprintf(stderr, "Invalid arguments!\nUsage: ./fileCompressor <flag> <path or file> |codebook|\n");
  fprintf(stderr, "flags:\n-R\tPerform a command (build/compress/decompress) recursively\n");
  fprintf(stderr, "\t\tusage: ./fileCompressor -R -b ./directoryname\n");
  fprintf(stderr, "-b\tBuilds a huffman codebook for a file or files\n");
  fprintf(stderr, "\t\tusage: ./fileCompressor -b ./filename.txt\n");
  fprintf(stderr, "-c\tCompresses a file or files given the codebook\n");
  fprintf(stderr, "\t\tusage: ./fileCompressor -R -c ./ ./HuffmanCodebook\n");
  fprintf(stderr, "-d\tDecompresses a file or files given the codebook\n");
  fprintf(stderr, "\t\tusage: ./fileCompressor -d ./filename.txt.hcz ./HuffmanCodebook\n");
}
