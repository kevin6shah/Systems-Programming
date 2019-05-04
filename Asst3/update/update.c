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
#define _GNU_SOURCE



typedef struct node{
  int version;
  unsigned char *code;
  char *filepath;
  char *filename;
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


 //function traverses project and generates linked list with information
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
        node *temp_node = malloc(sizeof(node));
        temp_node->version = 1;
        temp_node->code = malloc(SHA256_DIGEST_LENGTH*2 + 1);
        strcpy(temp_node->code, gethash(buffer));                   //hashcode
        temp_node->filepath = malloc(strlen(buffer) + 1);
        temp_node->next = NULL;
        strcpy(temp_node->filepath, buffer);                        //filepath
        temp_node->filename = malloc(temp->d_reclen + 1);
        strcpy(temp_node->filename, temp->d_name);                  //filename
        linked_list_ptr->next = temp_node;
        linked_list_ptr = temp_node;
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
    write(fd, ptr->filename, strlen(ptr->filename));
    write(fd, "\t", 1);
    write(fd, ptr->filepath, strlen(ptr->filepath));
    write(fd, "\t", 1);
    write(fd, ptr->code, strlen(ptr->code));
    write(fd, "\n", 1);
    ptr = ptr->next;
  }
  return 1;
}
node** createTable(){
    node** HashTable = malloc (150 * sizeof(node*));
    return HashTable;
}

int getkey(char *filename){
  int len = strlen(filename);
    int i = 0;
    int sum = 0;
    while(i < len){
        sum += (int)filename[i];
        i++;
    }
    int key = sum % 150;
    return key;
}

void nodeInsert(node* hashnode, node **table){
    int key = getkey(hashnode->filename);
    if(table[key] == NULL){
        table[key] = hashnode;
        return;
    }
    node* ptr = table[key];
    node* prev = table[key];
    while (ptr != NULL){
        prev = ptr;
        ptr = ptr->next;
    }
    prev->next = hashnode;
    return;


}

void update(char* project_name){

  //server-client interaction will happen here.
  //assumes that the .Manfiest was given.

  char* manifest_path = ".Manifest";
  int fd = open(".Manifest", O_RDONLY);
  if (fd < 0) {
    printf("Error creating the manifest file\n");
    return 0;
  }

  //next step is to create a hashtable (size 100), hashed to the name of the file
  node **HashTable = createTable();
  //this will store the server's .Manifest file
  //now to parse the file and actually store it inside

//read line by line
int line_number = 0;
int version_manifest;
int not_end_of_file = 1; //if 1, end of file;
while(not_end_of_file){
  char c;
  int num = 1000;
  char* line_buffer = malloc(num);
  int count = 0;
  while((not_end_of_file = read(fd, &c, 1)) > 0 && c != '\n'){
    line_buffer[count] = c;
    count++;
    if (count > num - 5) {
      num*=2;
      line_buffer = realloc(line_buffer, num);
    }
  }
  line_buffer[count] = '\0';
  if (strlen(line_buffer) < 1){
    break;
  }
  if (line_number == 0){
    version_manifest = atoi(line_buffer);
    line_number ++;
    continue;
  }

  node* hashnode = malloc(sizeof(node));

  //now we parse through each line.
  int tab_counter = 0;
  int index = 0;
  while(index < strlen(line_buffer)){
    int start_ind = index;;
    int end_ind = index;
    while (line_buffer[index] != '\t'){
        end_ind++;
        index++;
    }
    int size = (end_ind - start_ind) + 2;
    char* temp_string = malloc (size);
    strncpy(temp_string, line_buffer + start_ind, size -1);
    printf("temps: %s\n", temp_string);
    if (tab_counter == 0){
      hashnode->version = atoi(temp_string);
    } else if(tab_counter == 1){
      hashnode->filename = malloc(strlen(temp_string) + 1);
      strcpy(hashnode->filename, temp_string);
    } else if(tab_counter == 2){
      hashnode->filepath = malloc(strlen(temp_string) + 1);
      strcpy(hashnode->filepath, temp_string);
    } else if(tab_counter == 3){
      hashnode->code = malloc(strlen(temp_string) + 1);
      strcpy(hashnode->code, temp_string);
    }
    tab_counter++;
    index++;
    if(tab_counter > 3) break;
  }

  nodeInsert(hashnode, HashTable);



}


}

int main(int argc, char* argv[]){

  node *head = malloc(sizeof(node));
  linked_list_ptr = head;
  //printDir(argv[1]);
  //if (!createManfiest(head)) printf("FAILURE\n");
  update(argv[1]);



  return 0;

}
