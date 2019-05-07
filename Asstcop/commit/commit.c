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

 //has to request server for its current .Manifest file
 //generates a hash for every file in the project client side
 //compares it with each file in the server .Manifest


 //GAMEPLAN: take the server .Manfest and store the contents in a hash_client:
 // -> the name of the file will be whats "hashed"



 //makeManifest: makes a .manifest file from the project.
 //sets current version to zero:


 //function traverses project and generates linked list with information
 void make_list(char* filePath, node** hash_client) {
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
			make_list(buffer, hash_client);
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
        nodeInsert(temp_node, hash_client);
      }
    }
  }


node** createTable(){
    node** hash_client = malloc (150 * sizeof(node*));
    bzero(hash_client, sizeof(node*)*150);
    return hash_client;
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

  //next step is to create a hash_client (size 100), hashed to the name of the file
  node** hash_client = createTable();
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

  nodeInsert(hashnode, hash_client);

}

return hash_client;
}

void printHash(node** hash_client){
  int i;
  for (i = 0; i < 150; i++){
    if(hash_client[i] == NULL) continue;
    node* ptr = hash_client[i];
    for(;ptr != NULL; ptr = ptr->next){
      printf("filename: %s, key: %d\n", ptr->filename, i);

    }
  }
}


node* search_node(node *live_file, node** hash_client){
  int key = getkey(live_file->filename);
  node *pointer = hash_client[key]; //of hash_client
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


int commit(char* project_name){
  int server_manifest_version, client_manifest_version;
  node **hash_server = parse_manifest(".Manifest", &server_manifest_version);
  char path[255];
  strcpy(path, project_name);
  strcat(path, "/.Manifest");
  node **hash_client = parse_manifest(path, &client_manifest_version);

  if (server_manifest_version != client_manifest_version){
    printf("Versions do not match, please update local project\n");
    return 0;
  }
  node **live_hash = createTable();
  make_list(project_name, live_hash);
  //create a linked list which stores all the commits needed
  node *commit_list = malloc(sizeof(node));

  int i;
  int fd = open(".Commit", O_WRONLY | O_CREAT, 0700);
  if (fd < 0) {
    printf("Error creating the manifest file\n");
    return 0;
  }
  for (i = 0; i < 150; i++){
    if(hash_client[i] == NULL) continue;
    node* ptr = hash_client[i];
    for(;ptr != NULL; ptr = ptr->next){
      //temp holds the node to the .Manifest of live hash
      node* temp = search_node(ptr, live_hash);
      node* server_node = search_node(ptr, hash_server);

      if (strcmp(server_node->filename, "does not exist")==0){
        //do nothing, acceptable
        //file not in server yet, needs to be added
        // |change|<tab>|filepath|<newline>  change is either A=add or D=delete
        write(fd,"A",1);
        write(fd,"\t",1);
        write(fd, ptr->filepath, strlen(ptr->filepath));
        write(fd,"\n",1);

      } else if (strcmp(server_node->code, ptr->code)!=0 && server_node->version >= ptr->version){
        //error lol
        printf("client must synch with the repository before committing changes\n");
        remove(".Commit");
      }

      if(strcmp("does not exist", temp->filename)==0){
        //file does not exist in local project but exists in .Manifest huhuh
        printf("file (%s) does not exist in the local project\n", ptr->filepath);
      } else{
          if(strcmp(ptr->code, temp->code) != 0){
            //local file is different than file referenced in .manifest
            //write  |change <tab> incremented version number <tab> filepath <tab> updated hashedcode <newline> |
            int cversion = ptr->version + 1;
            char* version_buffer = malloc(100);
            sprintf(version_buffer, "%d", cversion);
            write(fd,"U",1);
            write(fd,"\t",1);
            write(fd, version_buffer, strlen(version_buffer));
            write(fd,"\t",1);
            write(fd, ptr->filepath, strlen(ptr->filepath));
            //write(fd,"\t",1);
            //write(fd, temp->code, strlen(temp->code));
            write(fd,"\n",1);
          }
        }
      }
    }

      int j;
      for (j = 0; j < 150; j++){
        if(hash_server[j] == NULL) continue;
        node* ptr = hash_server[i];
        for(;ptr != NULL; ptr = ptr->next){
          node* test = search_node(ptr, hash_client);
          if(strcmp(test->filename, "does not exist") == 0){
            //file exist in the server but not in client
            //has to be deleted
            write(fd,"D",1);
            write(fd,"\t",1);
            write(fd, ptr->filepath, strlen(ptr->filepath));
            write(fd,"\n",1);
          }
        }
      }

return 1;

}




int main(int argc, char* argv[]){



  if (!commit(argv[1])) {
    printf("Commit has failed...\n");
    return 0;
  }


  return 0;

}
