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
  char *filename;
  struct node* next;

}node;

 char* gethash(char* filepath){
     int fd = open(filepath, O_RDONLY);
   if (fd < 0) {
     printf("Incorrect Path Given... exiting ...\n");
     return NULL;
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

node** createTable(){
    node** HashTable = malloc (150 * sizeof(node*));
    bzero(HashTable, sizeof(node*)*150);
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

node** parse_manifest(char* manifest_path, int* version_num){

  //server-client interaction will happen here.
  //assumes that the .Manfiest was given.

  //char* manifest_path = ".Manifest";
  int fd = open(manifest_path, O_RDONLY);
  if (fd < 0) {
    printf("Can't open the manifest file\n");
    return 0;
  }

  //next step is to create a hashtable (size 100), hashed to the name of the file
  node** HashTable = createTable();
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
  line_buffer[count] = '\t';
  if (num == count) line_buffer = realloc(line_buffer, num + 2);
  line_buffer[++count] = '\0';
  if (strlen(line_buffer) < 1){
    break;
  }
  if (line_number == 0){
    version_manifest = atoi(line_buffer);
    *version_num = version_manifest;
    line_number ++;
    continue;
  }

  node* hashnode = malloc(sizeof(node));

  //now we parse through each line.

  if (strlen(line_buffer) < 2) continue;
  int i, j = 0, tabs_seen = 0;
  char token[PATH_MAX];
  for (i = 0; i < strlen(line_buffer); i++) {
    if (line_buffer[i] == '\t') {
      token[j] = '\0';
      tabs_seen++;
      if (tabs_seen == 1) {
        hashnode->version = atoi(token);
      } else if (tabs_seen == 2) {
        hashnode->filename = malloc(strlen(token) + 1);
        strcpy(hashnode->filename, token);
      } else if (tabs_seen == 3) {
        hashnode->filepath = malloc(strlen(token) + 1);
        strcpy(hashnode->filepath, token);
      } else if (tabs_seen == 4) {
        hashnode->code = malloc(strlen(token) + 1);
        strcpy(hashnode->code, token);
        break;
      } else break;
      j = 0;
      bzero(token, PATH_MAX);
      continue;
    }
    token[j] = line_buffer[i];
    j++;
  }

  nodeInsert(hashnode, HashTable);

}

return HashTable;
}


node* search_node(node *live_file, node** HashTable){
  int key = getkey(live_file->filename);
  node *pointer = HashTable[key]; //of hashtable
  while(pointer != NULL){
    if (strcmp(live_file->filename, pointer->filename)==0){
      if(strcmp(live_file->filepath, pointer->filepath)==0){
        //file exists
        return pointer;
      }
    }
    pointer = pointer->next;
  }
  node *temp = malloc(sizeof(node));
  temp->filename = "does not exist";
  return temp;
}

void printHash(node** HashTable){
  int i;
  for (i = 0; i < 150; i++){
    if(HashTable[i] == NULL) continue;
    node* ptr = HashTable[i];
    for(;ptr != NULL; ptr = ptr->next){
      printf("%d\t%s\t%s\t%s\n", ptr->version, ptr->filename, ptr->filepath, ptr->code);

    }
  }
}

int make_manifest(node** HashTable, char* path, int version) {
  int fd = open(path, O_WRONLY | O_CREAT, 0700);
  if (fd < 0) {
    printf("Could not create Manifest\n");
    return 0;
  }
  char v_num[250];
  sprintf(v_num, "%d\n", version);
  write(fd, v_num, strlen(v_num));
  int i;
  for (i = 0; i < 150; i++){
    if(HashTable[i] == NULL) continue;
    node* ptr = HashTable[i];
    for(;ptr != NULL; ptr = ptr->next){
      char v[250];
      sprintf(v, "%d\t", ptr->version);
      write(fd, v, strlen(v));
      write(fd, ptr->filename, strlen(ptr->filename));
      write(fd, "\t", 1);
      write(fd, ptr->filepath, strlen(ptr->filepath));
      write(fd, "\t", 1);
      write(fd, ptr->code, strlen(ptr->code));
      write(fd, "\n", 1);
    }
  }
  return 1;
}

char* get_name(char* file_path) {
  int index = -1, i;
  for (i = 0; i < strlen(file_path); i++) {
    if (file_path[i] == '/') index = i;
  }
  return file_path+index+1;
}

node* delete(char* filepath, node *front){
   // int size = front->size;
    node *head = front;
    node *ptr;
    node *prev = front;

    if (front == NULL){
        return head;
    }

    if (front->filepath == filepath && front->next == NULL){
        head = NULL;
        return head;
    }

    if (front->filepath == filepath){
        head = front->next;
        return head;
    }
    for (ptr = front; ptr != NULL; ptr = ptr->next){
        if (ptr->filepath != filepath){
            prev = ptr;
            continue;
        } else if (ptr->filepath == filepath){
            prev->next = ptr->next;
            return head;
        }
    }
    return head;
}

int remuuv(char* project_name, char* file_old_path){
  int client_manifest_version;
  char path[255];
  strcpy(path, project_name);
  strcat(path, "/.Manifest");
  node **hash_client = parse_manifest(path, &client_manifest_version);

  char* file_path = malloc(strlen(project_name) + strlen(file_old_path) + 2);
  strcpy(file_path, project_name);
  strcat(file_path, "/");
  strcat(file_path, file_old_path);

  node* result;
  node* tmp = malloc(sizeof(node));
  tmp->filepath = malloc(255);
  strcpy(tmp->filepath, file_path);
  tmp->filename = malloc(255);
  strcpy(tmp->filename, get_name(file_path));
  result = search_node(tmp, hash_client);

  if (strcmp(result->filename, "does not exist") != 0) {
    //Path exists on the manifest --> Remove it
    hash_client[getkey(result->filename)] = delete(result->filepath,hash_client[getkey(result->filename)]);
  } else {
    //Path does not exist --> Error
    printf("Error: Could not remove the file at '%s'\n", file_path);
    return 0;
  }
  remove(path);
  return make_manifest(hash_client, path, client_manifest_version);
}



int main(int argc, char* argv[]){

  if (!remuuv(argv[1], argv[2])) {
    printf("Remove has failed...\n");
    return 0;
  }


  return 0;

}
