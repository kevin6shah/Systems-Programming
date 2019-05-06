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

node* linked_list_ptr;

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

 void make_list(char* filePath, node** HashTable) {
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
			make_list(buffer, HashTable);
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
        //linked_list_ptr->next = temp_node;
        //linked_list_ptr = temp_node;
        nodeInsert(temp_node, HashTable);
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

void printHash(node** HashTable){
  int i;
  for (i = 0; i < 150; i++){
    if(HashTable[i] == NULL) continue;
    node* ptr = HashTable[i];
    for(;ptr != NULL; ptr = ptr->next){
      printf("filename: %s, key: %d\n", ptr->filename, i);

    }
  }
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


void update(char* project_name){
  node** live_hash = createTable();
  make_list(project_name, live_hash);
  int server_manifest_version, client_manifest_version;
  node **hash_server = parse_manifest(".Manifest", &server_manifest_version);
  char path[255];
  strcpy(path, project_name);
  strcat(path, "/.Manifest");
  node **hash_client = parse_manifest(path, &client_manifest_version);

  int fd = open(".Update", O_WRONLY | O_CREAT, 0700);
  if (fd < 0) {
    printf("Error creating the manifest file\n");
    return 0;
  }
  int i;

  int error = 0;
  for (i = 0; i < 150; i++){
    if(hash_client[i] == NULL) continue;
    node* file_client = hash_client[i];
    for(;file_client != NULL; file_client = file_client->next){
      int completed_if_else_command = 0;
      node* file_server = search_node(file_client, hash_server);
      node *ptr = search_node(file_client, live_hash);
      if(strcmp(ptr->filename, "does not exist") ==0){
        printf("file (%s) has been removed from the local project\n", file_client->filepath);
      }

      if (strcmp(file_server->filename, "does not exist") ==0){
        //in client but not in server
        if (server_manifest_version == client_manifest_version){
          //upload
          completed_if_else_command = 1;
          printf("U:\t%s\n", file_client->filepath);
        } else{
          //delete
          completed_if_else_command = 1;
          printf("D:\t%s\n", file_client->filepath);
          write(fd, "D\t", strlen("D\t"));
          char* version_num_buff = malloc(10);
          int tmp = file_client->version;
          sprintf(version_num_buff,"%d", tmp);
          write(fd, version_num_buff, sizeof(version_num_buff));
          write(fd, "\t", 1);
          write(fd, file_client->filename, strlen(file_client->filename));
          write(fd, "\t", 1);
          write(fd, file_client->filepath, strlen(file_client->filepath));
          write(fd, "\t", 1);
          write(fd, file_client->code, strlen(file_client->code));
          write(fd, "\n", 1);

        }
      } else if(strcmp(file_server->filename, "does not exist") !=0){
          //file exists in both the server and client manifests
         if(server_manifest_version == client_manifest_version && strcmp(file_server->code, ptr->code) != 0) {
          //upload
          completed_if_else_command = 1;
          printf("U:\t%s\n", file_client->filepath);
        } else if(server_manifest_version != client_manifest_version && file_client->version != file_server->version){
            //modify
            completed_if_else_command = 1;
            printf("M:\t%s\n", file_client->filepath);
            write(fd, "M\t", strlen("M\t"));
            char* version_num_buff = malloc(10);
            sprintf(version_num_buff,"%d", file_server->version);
            write(fd, version_num_buff, sizeof(version_num_buff));
            write(fd, "\t", 1);
            write(fd, file_client->filename, strlen(file_client->filename));
            write(fd, "\t", 1);
            write(fd, file_client->filepath, strlen(file_client->filepath));
            write(fd, "\t", 1);
            write(fd, file_server->code, strlen(file_server->code));
            write(fd, "\n", 1);
        } else if(server_manifest_version == client_manifest_version && file_client->version == file_server->version && strcmp(file_client->code, file_server->code)==0){
            completed_if_else_command = 1;
        }

      } else if (!completed_if_else_command){
        error = 1;
        printf("M:\t%s\n", file_client->filepath);


      }
    }
  }

  //now to see what files from the server are not in the client.

    int j;
    for (j = 0; j < 150; j++){
      if(hash_server[j] == NULL) continue;
      node* ptr = hash_server[i];
      for(;ptr != NULL; ptr = ptr->next){
        node* test = search_node(ptr, hash_client);
        if(strcmp(test->filename, "does not exist") == 0){
          printf("A:\t%s\n", ptr->filepath);
          write(fd, "A\t", strlen("A\t"));
          char* version_num_buff = malloc(10);
          sprintf(version_num_buff,"%d", ptr->version);
          write(fd, version_num_buff, sizeof(version_num_buff));
          write(fd, "\t", 1);
          write(fd, ptr->filename, strlen(ptr->filename));
          write(fd, "\t", 1);
          write(fd, ptr->filepath, strlen(ptr->filepath));
          write(fd, "\t", 1);
          write(fd, ptr->code, strlen(ptr->code));
          write(fd, "\n", 1);

        }
      }
    }




}




int main(int argc, char* argv[]){


  update(argv[1]);


  return 0;

}
