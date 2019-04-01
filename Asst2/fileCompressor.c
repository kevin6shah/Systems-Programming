#include "compressor.h"
#include "data.h"

// TODO: If the given file is empty or does not exist
// TODO: Recursive decompress and decompress

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

  char* path = malloc(strlen(huffmanPath));
  strcpy(path, huffmanPath);
  makeCodeBook(path, book);
}

void buildRecursiveHelper(char* filePath, hashnode **HASHTABLE, int *cap) {
  DIR *directory = opendir(filePath);
	struct dirent* temp;
	if (directory == NULL) {
		printf("Could not find the directory!\n");
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
        char *buffer2 = malloc(len);
        strcpy(buffer2, findBuffer(buffer));
        *cap = *cap + tokenize(HASHTABLE, buffer2, len);
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

  char* path = malloc(strlen(huffmanPath));
  strcpy(path, huffmanPath);
  makeCodeBook(path, book);
}

hashnode** compressInit(char* pathFile, char* pathHuffbook) {
    hashnode** table = createTable();
    int len = 0;
    len = bufferSize(pathHuffbook) + 1;
    if (len == 0) return;
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
    char* file = malloc(len);
    strcpy(file, findBuffer(pathFile));

    //now to actually compress into new file
    char *writePath = malloc (strlen(pathFile) + 5);
    strcpy(writePath,pathFile);
    strcat(writePath, ".hcz");

    int fd = open(writePath, O_WRONLY|O_CREAT, 0700);
    if (fd < 0) {
        printf("Error could not create the file!\n");
        return;
    }
    int i = 0, startIndex = 0, endIndex = 0;
    while (i < strlen(file)-1){
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
		printf("Could not find the directory!\n");
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
        compress(filePath, table);
      }
    }
    i++;
  }
}

int isDir(char* path) {
  DIR *directory = opendir(path);
  if (directory == NULL) {
    close(directory);
    return 0;
  } else {
    close(directory);
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

int main(int argc, char* argv[]) {
    if (!(argc > 2 && argc < 6)) {
      printf("Invalid arguments!\nUsage: ./fileCompressor <flag> <path or file> |codebook|\n");
      return 0;
    }

    // One one possible case would be to build. Ex: ./fileCompressor -b ./
    if (argc == 3) {
      //                  REGULAR BUILD
      if (strcmp(argv[1], "-b") == 0) build(argv[2], "./");
      else {
        printf("Invalid arguments!\nUsage: ./fileCompressor <flag> <path or file> |codebook|\n");
        return 0;
      }
    }

    // Possible Cases:
    // ./fileCompressor -c ./test.txt ./HuffmanCodebook
    // ./fileCompressor -d ./test.txt.hcz ./HuffmanCodebook
    // ./fileCompressor -R -b ./test.txt
    if (argc == 4) {
      //                  REGULAR COMPRESSION
      if (strcmp(argv[1], "-c") == 0) {
        if (isDir(argv[2])) {
          printf("Error: %s is a directory\nTerminating the program!", argv[2]);
          return 0;
        }
        if (isHuffman(argv[3]) == 0) {
          printf("Error: The Huffman file is incorrect or corrupted!\n");
          return 0;
        }
        hashnode **table = compressInit(argv[2], argv[3]);
        compress(argv[2], table);
      }

      //                  REGULAR DECOMPRESSION
      else if (strcmp(argv[1], "-d") == 0) /*decompress = 1*/;

      //                  RECURSIVE BUILD
      else if ((strcmp(argv[1], "-R") == 0 && strcmp(argv[2], "-b") == 0) ||
      (strcmp(argv[1], "-b") == 0 && strcmp(argv[2], "-R") == 0)) {
        if (isDir(argv[3]) == 0) {
          printf("Not a directory: Building Codebook in regular mode\n");
          build(argv[3], "./");
        } else buildRecursive(argv[3], "./");
      }

      //                      ALL ELSE
      else {
        printf("Invalid arguments!\nUsage: ./fileCompressor <flag> <path or file> |codebook|\n");
        return 0;
      }
    }

    // Possible Cases:
    // ./fileCompressor -R -c ./test.txt ./HuffmanCodebook
    // ./fileCompressor -R -d ./test.txt.hcz ./HuffmanCodebook

    // Potential Checks:
    // -R flag and -c or -d flag interchanged
    // If it is a directory or a file (print warning) -> continue program
    if (argc == 5) {
      //                  RECURSIVE COMPRESSION
      if ((strcmp(argv[1], "-R") == 0 && strcmp(argv[2], "-c") == 0) ||
      (strcmp(argv[1], "-c") == 0 && strcmp(argv[2], "-R") == 0)) {
        if (isHuffman(argv[4]) == 0) {
          printf("Error: The Huffman file is incorrect or corrupted!\n");
          return 0;
        }
        if (isDir(argv[3]) == 0) {
          printf("Not a directory: Compressing in regular mode\n");
          printf("%s %s\n", argv[3], argv[4]);
          hashnode **table = compressInit(argv[3], argv[4]);
          compress(argv[3], table);
          return 0;
        }
        hashnode **table = compressInit(argv[3], argv[4]);
        compressRecursive(argv[3], table);
      }

      //                  RECURSIVE DECOMPRESSION
      else if ((strcmp(argv[1], "-R") == 0 && strcmp(argv[2], "-d") == 0) ||
      (strcmp(argv[1], "-d") == 0 && strcmp(argv[2], "-R") == 0)) {
        //recursive = 1;
        //decompress = 1;
      }

      //                          ALL ELSE
      else {
        printf("Invalid arguments!\nUsage: ./fileCompressor <flag> <path or file> |codebook|\n");
        return 0;
      }
    }
    return 0;
}
