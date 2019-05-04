#include <openssl/sha.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <sys/types.h>
 #include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

// Add -lcrypto while compiling
 int main(int argc, char** argv) {
   if (argc != 2) {
     printf("Usage: %s <path of file to encrypt>\n", argv[0]);
     return 0;
   }
   int fd = open(argv[1], O_RDONLY);
   if (fd < 0) {
     printf("Incorrect Path Given... exiting ...\n");
     return 0;
   }

   struct stat stats;
   fstat(fd, &stats);
   int size = stats.st_size;
   char *buf = malloc(size+1);
   int n = read(fd, buf, size);

   unsigned char tmphash[SHA256_DIGEST_LENGTH];
   SHA256(buf, size, tmphash);
   unsigned char hash[SHA256_DIGEST_LENGTH*2];

   int i;
   for (i = 0; i < SHA256_DIGEST_LENGTH; i++) {
    sprintf((char*)&(hash[i*2]), "%02x", tmphash[i]);
   }

   printf("Hash: %s\n", hash);
   return 0;
 }
