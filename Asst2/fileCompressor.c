#include "compressor.h"
#include "data.h"

int main(int argc, char* argv[]) {

    // In case incorrect amount of arguments are given
    if (!(argc > 2 && argc < 6)) {
      printStatements();
      return 0;
    }

    // One one possible case would be to build. Ex: ./fileCompressor -b ./
    if (argc == 3) {
      //                  REGULAR BUILD
      if (strcmp(argv[1], "-b") == 0) {
        if (isDir(argv[2])) {
          fprintf(stderr, "Error: %s is a directory\nTerminating the program!\n", argv[2]);
          fprintf(stderr, "Usage: Try running the same command with the -R flag\n");
          return 0;
        }
        build(argv[2], "./");
      } else {
        printStatements();
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
          fprintf(stderr, "Error: %s is a directory\nTerminating the program!\n", argv[2]);
          fprintf(stderr, "Usage: Try running the same command with the -R flag\n");
          return 0;
        }
        if (isHuffman(argv[3]) == 0) {
          fprintf(stderr, "Error: The Huffman file is incorrect or corrupted!\n");
          return 0;
        }
        hashnode **table = compressInit(argv[2], argv[3]);
        if (table == NULL && bufferSize(argv[2]) == 0) {
          char *writePath = malloc (strlen(argv[2]) + 5);
          strcpy(writePath,argv[2]);
          strcat(writePath, ".hcz");
          int fd = open(writePath, O_WRONLY|O_CREAT, 0700);
          fprintf(stderr, "Warning: Tried compressing an empty file!\nEmpty compressed file created!\n");
          return 0;
        }
        compress(argv[2], table);
      }

      //                  REGULAR DECOMPRESSION
      else if (strcmp(argv[1], "-d") == 0) {
        if (isDir(argv[2])) {
          fprintf(stderr, "Error: %s is a directory\nTerminating the program!\n", argv[2]);
          fprintf(stderr, "Usage: Try running the same command with the -R flag\n");
          return 0;
        }
        if (isHuffman(argv[3]) == 0) {
          fprintf(stderr, "Error: The Huffman file is incorrect or corrupted!\n");
          return 0;
        }
        treeNode* tree = decompressInit(argv[2], argv[3]);
        if (tree == NULL && bufferSize(argv[2]) == 0) {
          char *writePath = malloc (strlen(argv[2]) -4);
          strncpy(writePath, argv[2], strlen(argv[2])-4);
          writePath[strlen(argv[2])-4] = '\0';
          int fd = open(writePath, O_WRONLY|O_CREAT, 0700);
          fprintf(stderr, "Warning: Tried decompressing an empty file!\nEmpty decompressed file created!\n");
          return 0;
        }
        decompress(argv[2], tree);
      }

      //                  RECURSIVE BUILD
      else if ((strcmp(argv[1], "-R") == 0 && strcmp(argv[2], "-b") == 0) ||
      (strcmp(argv[1], "-b") == 0 && strcmp(argv[2], "-R") == 0)) {
        if (isDir(argv[3]) == 0) {
          fprintf(stderr, "Not a directory: Building Codebook in regular mode\n");
          build(argv[3], "./");
        } else buildRecursive(argv[3], "./");
      }

      //                      ALL ELSE
      else {
        printStatements();
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
          fprintf(stderr, "Error: The Huffman file is incorrect or corrupted!\n");
          return 0;
        }
        if (isDir(argv[3]) == 0) {
          fprintf(stderr, "Not a directory: Compressing in regular mode\n");
          hashnode **table = compressInit(argv[3], argv[4]);
          compress(argv[3], table);
          return 0;
        }
        hashnode **table = compressInit(argv[3], argv[4]);
        if (table == NULL && bufferSize(argv[3]) == 0) {
          char *writePath = malloc (strlen(argv[3]) + 5);
          strcpy(writePath,argv[3]);
          strcat(writePath, ".hcz");
          int fd = open(writePath, O_WRONLY|O_CREAT, 0700);
          fprintf(stderr, "Warning: Tried compressing an empty file!\nEmpty compressed file created!\n");
          return 0;
        }
        compressRecursive(argv[3], table);
      }

      //                  RECURSIVE DECOMPRESSION
      else if ((strcmp(argv[1], "-R") == 0 && strcmp(argv[2], "-d") == 0) ||
      (strcmp(argv[1], "-d") == 0 && strcmp(argv[2], "-R") == 0)) {
        if (isHuffman(argv[4]) == 0) {
          fprintf(stderr, "Error: The Huffman file is incorrect or corrupted!\n");
          return 0;
        }
        if (isDir(argv[3]) == 0) {
          fprintf(stderr, "Not a directory: Decompressing in regular mode\n");
          treeNode* tree = decompressInit(argv[3], argv[4]);
          decompress(argv[3], tree);
          return 0;
        }
        treeNode* tree = decompressInit(argv[3], argv[4]);
        decompressRecursive(argv[3], tree);
      }

      //                          ALL ELSE
      else {
        printStatements();
        return 0;
      }
    }
    return 0;
}
