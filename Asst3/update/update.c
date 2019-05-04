#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <signal.h>
#include <openssl/sha.h>



typedef struct node{
  int version;
  unsigned char *code;
  char *filepath;
  struct node* next;

}node;

node* linked_list_ptr;

 char* gethash(char* filepath){
     int fd = open(filepath, O_RDONLY);
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
   unsigned char *hash = malloc (SHA256_DIGEST_LENGTH*2);

   int i;
   for (i = 0; i < SHA256_DIGEST_LENGTH; i++) {
    sprintf((char*)(hash+(i*2)), "%02x", tmphash[i]);
   }

   return hash;
 }

 //has to request server for its current .Manifest file
 //generates a hash for every file in the project client side
 //compares it with each file in the server .Manifest


 //GAMEPLAN: take the server .Manfest and store the contents in a hashtable:
 // -> the name of the file will be whats "hashed"



 //makeManifest: makes a .manifest file from the project.
 //sets current version to zero:


 //function traverses project, prints file paths
 void printDir(char* filePath) {
  DIR *directory = opendir(filePath);
	struct dirent* temp;
	if (directory == NULL) {
		fprintf(stderr, "Could not find the directory!\n");
		return;
	}

  while ((temp = readdir(directory)) != NULL) {
    if (temp->d_type == 4 && temp->d_name[0] != '.') {
      char* buffer = malloc(2+temp->d_reclen+strlen(filePath));
			strcpy(buffer, filePath);
			strcat(buffer, "/");
			strcat(buffer, temp->d_name);
			printDir(buffer);
    }
    else if (temp->d_name[0] != '.') {

      int count = 3+strlen(filePath)+temp->d_reclen;
				char* buffer = malloc(count);
				strcpy(buffer, filePath);
				strcat(buffer, "/");
				strcat(buffer, temp->d_name);
        node *temp = malloc(sizeof(node));
        temp->version = 1;
        temp->code = malloc(SHA256_DIGEST_LENGTH*2 + 1);
        strcpy(temp->code, gethash(buffer));
        temp->filepath = malloc(strlen(buffer) + 1);
        temp->next = NULL;
        strcpy(temp->filepath, buffer);
        linked_list_ptr->next = temp;
        linked_list_ptr = temp;
      }
    }

  }

int createManfiest(node *head){
  int fd = open(".Manifest", O_WRONLY | O_CREAT, 0700);
  if (fd < 0) {
    printf("Error creating the manifest file\n");
    return 0;
  }
  node *ptr = head->next;
  char version[255];
  sprintf(version, "%d",ptr->version);
  write(fd, version, strlen(version));
  write(fd, "\n", 1);
  while (ptr != NULL) {
    bzero(version , 255);
    sprintf(version, "%d",ptr->version);
    write(fd, version , strlen(version));
    write(fd, "\t", 1);
    write(fd, ptr->filepath, strlen(ptr->filepath));
    write(fd, "\t", 1);
    write(fd, ptr->code, strlen(ptr->code));
    write(fd, "\n", 1);
    ptr = ptr->next;
  }
  return 1;
}



int main(int argc, char* argv[]){

  node *head = malloc(sizeof(node));
  linked_list_ptr = head;
  printDir(argv[1]);
  if (!createManfiest(head)) printf("FAILURE\n");



  return 0;

}
